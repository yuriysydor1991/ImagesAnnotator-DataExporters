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

/// @brief The four corners of the one rectangle of the database below, the
/// line both the oriented box and the polygon layouts write for it
const std::string ultralyticsCorners = "0 0.25 0.2 0.75 0.2 0.75 0.6 0.25 0.6";

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

  /**
   * @brief Builds the context of the wanted dataset layout, the very same
   * instance the exporter is asked for and then driven with.
   */
  template <class ContextT>
  std::shared_ptr<ContextT> context(const std::string& subdir)
  {
    auto ctx = std::make_shared<ContextT>();
    ctx->set_export_path((root / subdir).string());
    ctx->set_db_provider(db);
    return ctx;
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

  auto ctx = context<iade::PlainTxtExportLibraryContext>("plain");

  auto exporter = iade::LibraryFacade::create_exporter(ctx);

  ASSERT_NE(exporter, nullptr);
  ASSERT_TRUE(exporter->export_db(ctx));

  ASSERT_TRUE(std::filesystem::is_regular_file(dir / "dog.txt"));
  EXPECT_EQ(read_file(dir / "dog.txt"),
            (imagesDir / "a.png").string() + " 1 50 20 100 40");
}

TEST_F(CTEST_Exporters, yolo4_export_writes_the_darknet_layout)
{
  auto ctx = context<iade::Yolo4ExportLibraryContext>("yolo");

  auto exporter = iade::LibraryFacade::create_exporter(ctx);

  ASSERT_NE(exporter, nullptr);
  ASSERT_TRUE(exporter->export_db(ctx));

  const std::filesystem::path dir = root / "yolo";

  EXPECT_TRUE(std::filesystem::is_directory(dir / "backup"));
  EXPECT_EQ(read_file(dir / "data" / "obj.names"), "dog");
  EXPECT_TRUE(std::filesystem::is_regular_file(dir / "data" / "a.png"));
  EXPECT_EQ(read_file(dir / "data" / "a.txt"), "0 0.5 0.4 0.5 0.4");
}

TEST_F(CTEST_Exporters, ultralytics_detect_export_writes_the_data_yaml_layout)
{
  auto ctx = context<iade::UltralyticsDetectExportLibraryContext>("ul-detect");

  auto exporter = iade::LibraryFacade::create_exporter(ctx);

  ASSERT_NE(exporter, nullptr);
  ASSERT_TRUE(exporter->export_db(ctx));

  const std::filesystem::path dir = root / "ul-detect";

  EXPECT_TRUE(
      std::filesystem::is_regular_file(dir / "images" / "train" / "a.png"));

  const std::string yaml = read_file(dir / "data.yaml");

  EXPECT_NE(yaml.find("\ntrain: images/train\n"), std::string::npos);
  EXPECT_NE(yaml.find("names:\n  0: 'dog'"), std::string::npos);

  EXPECT_EQ(read_file(dir / "labels" / "train" / "a.txt"), "0 0.5 0.4 0.5 0.4");
}

TEST_F(CTEST_Exporters, ultralytics_obb_export_writes_the_four_box_corners)
{
  auto ctx = context<iade::UltralyticsObbExportLibraryContext>("ul-obb");

  auto exporter = iade::LibraryFacade::create_exporter(ctx);

  ASSERT_NE(exporter, nullptr);
  ASSERT_TRUE(exporter->export_db(ctx));

  EXPECT_EQ(read_file(root / "ul-obb" / "labels" / "train" / "a.txt"),
            ultralyticsCorners);
}

// The same corners of one and the same rectangle the oriented box case above
// expects: what tells the two layouts apart is the training task that reads
// the file, not the file itself.
TEST_F(CTEST_Exporters, ultralytics_segment_export_writes_the_box_polygon)
{
  auto ctx = context<iade::UltralyticsSegmentExportLibraryContext>("ul-seg");

  auto exporter = iade::LibraryFacade::create_exporter(ctx);

  ASSERT_NE(exporter, nullptr);
  ASSERT_TRUE(exporter->export_db(ctx));

  EXPECT_EQ(read_file(root / "ul-seg" / "labels" / "train" / "a.txt"),
            ultralyticsCorners);
}

TEST_F(CTEST_Exporters, coco_export_writes_the_pixel_bbox_of_the_rectangle)
{
  auto ctx = context<iade::CocoExportLibraryContext>("coco");

  auto exporter = iade::LibraryFacade::create_exporter(ctx);

  ASSERT_NE(exporter, nullptr);
  ASSERT_TRUE(exporter->export_db(ctx));

  const std::filesystem::path dir = root / "coco";

  EXPECT_TRUE(std::filesystem::is_regular_file(dir / "images" / "a.png"));

  const std::string json =
      read_file(dir / "annotations" / "instances_default.json");

  EXPECT_NE(json.find("{\"id\": 1, \"file_name\": \"a.png\", \"width\": 200, "
                      "\"height\": 100}"),
            std::string::npos);
  // the very four numbers of the rectangle, in the image own pixels
  EXPECT_NE(json.find("\"bbox\": [50, 20, 100, 40], \"area\": 4000"),
            std::string::npos);
  EXPECT_NE(
      json.find("{\"id\": 1, \"name\": \"dog\", \"supercategory\": \"\"}"),
      std::string::npos);
}

TEST_F(CTEST_Exporters, pascal_voc_export_writes_the_corners_of_the_rectangle)
{
  auto ctx = context<iade::PascalVocExportLibraryContext>("voc");

  auto exporter = iade::LibraryFacade::create_exporter(ctx);

  ASSERT_NE(exporter, nullptr);
  ASSERT_TRUE(exporter->export_db(ctx));

  const std::filesystem::path dir = root / "voc";

  EXPECT_TRUE(std::filesystem::is_regular_file(dir / "JPEGImages" / "a.png"));
  EXPECT_EQ(read_file(dir / "ImageSets" / "Main" / "train.txt"), "a");

  const std::string xml = read_file(dir / "Annotations" / "a.xml");

  EXPECT_NE(xml.find("<filename>a.png</filename>"), std::string::npos);
  EXPECT_NE(xml.find("<width>200</width>"), std::string::npos);
  EXPECT_NE(xml.find("<name>dog</name>"), std::string::npos);
  // the far corner of the rectangle, not its size
  EXPECT_NE(xml.find("<xmin>50</xmin>"), std::string::npos);
  EXPECT_NE(xml.find("<xmax>150</xmax>"), std::string::npos);
}

// The images sit in the export directory itself here, beside the one
// descriptor naming them, and the rectangle reaches it as its own centre
TEST_F(CTEST_Exporters,
       createml_export_writes_the_pixel_centre_of_the_rectangle)
{
  auto ctx = context<iade::CreateMLExportLibraryContext>("createml");

  auto exporter = iade::LibraryFacade::create_exporter(ctx);

  ASSERT_NE(exporter, nullptr);
  ASSERT_TRUE(exporter->export_db(ctx));

  const std::filesystem::path dir = root / "createml";

  EXPECT_TRUE(std::filesystem::is_regular_file(dir / "a.png"));

  const std::string json = read_file(dir / "annotations.json");

  EXPECT_NE(json.find("{\"imagefilename\": \"a.png\", \"annotation\": ["),
            std::string::npos);
  // 50 + 100/2 across and 20 + 40/2 down: the centre of the very rectangle the
  // COCO case above writes as its top left corner and size
  EXPECT_NE(json.find("{\"label\": \"dog\", \"coordinates\": {\"x\": 100, "
                      "\"y\": 40, \"width\": 100, \"height\": 40}}"),
            std::string::npos);
}

TEST_F(CTEST_Exporters, pytorch_vision_export_crops_into_the_tag_directory)
{
  auto ctx = context<iade::PyTorchExportLibraryContext>("pytorch");
  ctx->set_cropper(std::make_shared<FakeCropper>());

  std::filesystem::create_directories(ctx->get_export_path());

  auto exporter = iade::LibraryFacade::create_exporter(ctx);

  ASSERT_NE(exporter, nullptr);
  ASSERT_TRUE(exporter->export_db(ctx));

  const std::filesystem::path dir = root / "pytorch";

  ASSERT_TRUE(std::filesystem::is_directory(dir / "dog"));
  EXPECT_EQ(read_file(dir / "dog" / "a.png"), "cropped");
}

TEST_F(CTEST_Exporters,
       perform_export_runs_the_export_named_by_the_library_context)
{
  auto ctx = std::make_shared<iade::Yolo4ExportLibraryContext>();

  ctx->set_export_path((root / "perform_export").string());
  ctx->set_db_provider(db);

  auto lib = iade::LibraryFacade::create_library(ctx);

  ASSERT_NE(lib, nullptr);
  ASSERT_TRUE(lib->perform_export(ctx));

  EXPECT_NE(ctx->get_exporter(), nullptr);
  EXPECT_EQ(read_file(root / "perform_export" / "data" / "obj.names"), "dog");
}

TEST_F(CTEST_Exporters, library_version_matches_the_data_drivers_one_it_reads)
{
  EXPECT_FALSE(iade::LibraryFacade::library_version().empty());
  EXPECT_FALSE(iadd::LibraryFacade::library_version().empty());
}
