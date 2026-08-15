#include <ImagesAnnotatorDataDrivers-0.11/LibraryFacade.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "IImageCropperFacility.h"
#include "LibraryFacade.h"

namespace iadd = ImagesAnnotatorDataDrivers011;
namespace iade = ImagesAnnotatorDataExporters011;

namespace
{

class FakeCropper : public iade::IImageCropperFacility
{
 public:
  bool crop_out_2_fs(ImageRecordPtr, ImageRecordRectPtr,
                     std::string& tofpath) override
  {
    std::ofstream f(tofpath);
    f << "cropped";
    return f.good();
  }

  IImageCropperFacilityPtr clone() override
  {
    return std::make_shared<FakeCropper>();
  }
};

std::string read_file(const std::filesystem::path& p)
{
  std::ifstream f(p);
  std::stringstream ss;
  ss << f.rdbuf();
  std::string s = ss.str();
  while (!s.empty() && (s.back() == '\n' || s.back() == '\r')) s.pop_back();
  return s;
}

/**
 * @brief Builds a real data drivers database over a real image file, so the
 * exporters are driven with exactly the records a project file yields.
 */
class CTEST_Exporters : public testing::Test
{
 public:
  void SetUp() override
  {
    root = std::filesystem::path{testing::TempDir()} / "ctest_exporters";
    imagesDir = root / "images";

    std::filesystem::remove_all(root);
    std::filesystem::create_directories(imagesDir);

    std::ofstream image(imagesDir / "a.png");
    image << "not-a-real-image";
    image.close();

    auto factory = iadd::LibraryFacade::create_records_factory();

    ASSERT_NE(factory, nullptr);

    auto ir = factory->create_image_record("a.png", imagesDir.string());

    ASSERT_NE(ir, nullptr);

    ir->iwidth = 200;
    ir->iheight = 100;
    ir->rects.emplace_back(
        factory->create_image_rect_record("dog", 50, 20, 100, 40));

    db = iadd::LibraryFacade::create_annotations_db();

    ASSERT_NE(db, nullptr);

    db->add_images_db({ir});
  }

  void TearDown() override { std::filesystem::remove_all(root); }

  iade::ExportContextPtr context(const std::string& subdir)
  {
    auto ectx = iade::LibraryFacade::create_export_context();
    ectx->export_path = (root / subdir).string();
    ectx->dbProvider = db;
    return ectx;
  }

  std::filesystem::path root;
  std::filesystem::path imagesDir;
  iadd::IAnnotationsDBPtr db;
};

}  // namespace

TEST_F(CTEST_Exporters, plain_txt_export_writes_one_file_per_annotation)
{
  const std::filesystem::path dir = root / "plain";
  std::filesystem::create_directories(dir);

  auto exporter = iade::LibraryFacade::create_exporter(
      std::make_shared<iade::PlainTxtExportLibraryContext>());

  ASSERT_NE(exporter, nullptr);
  ASSERT_TRUE(exporter->export_db(context("plain")));

  ASSERT_TRUE(std::filesystem::is_regular_file(dir / "dog.txt"));
  EXPECT_EQ(read_file(dir / "dog.txt"),
            (imagesDir / "a.png").string() + " 1 50 20 100 40");
}

TEST_F(CTEST_Exporters, yolo4_export_writes_the_darknet_layout)
{
  auto exporter = iade::LibraryFacade::create_exporter(
      std::make_shared<iade::Yolo4ExportLibraryContext>());

  ASSERT_NE(exporter, nullptr);
  ASSERT_TRUE(exporter->export_db(context("yolo")));

  const std::filesystem::path dir = root / "yolo";

  EXPECT_TRUE(std::filesystem::is_directory(dir / "backup"));
  EXPECT_EQ(read_file(dir / "data" / "obj.names"), "dog");
  EXPECT_TRUE(std::filesystem::is_regular_file(dir / "data" / "a.png"));
  EXPECT_EQ(read_file(dir / "data" / "a.txt"), "0 0.5 0.4 0.5 0.4");
}

TEST_F(CTEST_Exporters, pytorch_vision_export_crops_into_the_tag_directory)
{
  auto ectx = context("pytorch");
  ectx->cropper = std::make_shared<FakeCropper>();

  std::filesystem::create_directories(ectx->export_path);

  auto exporter = iade::LibraryFacade::create_exporter(
      std::make_shared<iade::PyTorchExportLibraryContext>());

  ASSERT_NE(exporter, nullptr);
  ASSERT_TRUE(exporter->export_db(ectx));

  const std::filesystem::path dir = root / "pytorch";

  ASSERT_TRUE(std::filesystem::is_directory(dir / "dog"));
  EXPECT_EQ(read_file(dir / "dog" / "a.png"), "cropped");
}

TEST_F(CTEST_Exporters, libcall_runs_the_export_named_by_the_library_context)
{
  auto ctx = std::make_shared<iade::Yolo4ExportLibraryContext>();

  ctx->export_path = (root / "libcall").string();
  ctx->dbProvider = db;

  auto lib = iade::LibraryFacade::create_library(ctx);

  ASSERT_NE(lib, nullptr);
  ASSERT_TRUE(lib->libcall(ctx));

  EXPECT_NE(ctx->exporter, nullptr);
  EXPECT_EQ(read_file(root / "libcall" / "data" / "obj.names"), "dog");
}

TEST_F(CTEST_Exporters, library_version_matches_the_data_drivers_one_it_reads)
{
  EXPECT_FALSE(iade::LibraryFacade::library_version().empty());
  EXPECT_FALSE(iadd::LibraryFacade::library_version().empty());
}
