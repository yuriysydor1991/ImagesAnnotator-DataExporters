#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <memory>
#include <string>

#include "src/croppers/ImageCropperFactory.h"
#include "src/exporters/PyTorchVisionFolderExporter.h"

using namespace testing;
using iannotator::exporters::ImageRecord;
using iannotator::exporters::ImageRecordPtr;
using iannotator::exporters::ImageRecordRect;
using iannotator::exporters::ImageRecordRectPtr;
using iannotator::exporters::ImageRecordsSet;
using iannotator::exporters::PyTorchExportLibraryContext;
using iannotator::exporters::PyTorchVisionFolderExporter;

namespace
{

class FakeProvider : public iannotator::exporters::IImagesPathsDBProvider
{
 public:
  ImageRecordsSet records;

  ImageRecordsSet& get_images_db() override { return records; }
  std::string get_db_path() override { return {}; }
  AnnotationsList get_available_annotations() override { return {}; }
  bool changed() override { return false; }
  bool delete_image_record(const std::string&) override { return false; }
};

class FakeCropper : public iannotator::exporters::IImageCropperFacility
{
 public:
  bool crop_out_2_fs(ImageRecordPtr, ImageRecordRectPtr,
                     std::string& tofpath) override
  {
    croppedTo.emplace_back(tofpath);
    return true;
  }

  IImageCropperFacilityPtr clone() override
  {
    return std::make_shared<FakeCropper>();
  }

  std::vector<std::string> croppedTo;
};

}  // namespace

// An export left without a cropper is only an error when this build has no
// cropper of its own to fall back on. Branching at the run time rather than on
// the preprocessor keeps the case registered - and real - in both builds.
TEST(UTEST_PyTorchVisionFolderExporter, handles_a_missing_cropper_by_the_build)
{
  const std::filesystem::path dir =
      std::filesystem::path{testing::TempDir()} / "utest_pytorch_no_cropper";
  std::filesystem::remove_all(dir);
  std::filesystem::create_directories(dir);

  auto ctx = std::make_shared<PyTorchExportLibraryContext>();
  ctx->set_export_path(dir.string());
  ctx->set_db_provider(std::make_shared<FakeProvider>());
  ctx->set_cropper(nullptr);

  const bool hasBuiltin =
      iannotator::exporters::croppers::create_builtin_cropper() != nullptr;

  PyTorchVisionFolderExporter exporter;

  EXPECT_EQ(exporter.export_db(ctx), hasBuiltin);

  std::filesystem::remove_all(dir);
}

TEST(UTEST_PyTorchVisionFolderExporter, fails_when_export_path_is_empty)
{
  auto ctx = std::make_shared<PyTorchExportLibraryContext>();
  ctx->set_export_path("");
  ctx->set_db_provider(std::make_shared<FakeProvider>());
  ctx->set_cropper(std::make_shared<FakeCropper>());

  PyTorchVisionFolderExporter exporter;

  EXPECT_FALSE(exporter.export_db(ctx));
}

TEST(UTEST_PyTorchVisionFolderExporter, succeeds_for_a_db_without_records)
{
  const std::filesystem::path dir =
      std::filesystem::path{testing::TempDir()} / "utest_pytorch_export";
  std::filesystem::remove_all(dir);
  std::filesystem::create_directories(dir);

  auto ctx = std::make_shared<PyTorchExportLibraryContext>();
  ctx->set_export_path(dir.string());
  ctx->set_db_provider(std::make_shared<FakeProvider>());
  ctx->set_cropper(std::make_shared<FakeCropper>());

  PyTorchVisionFolderExporter exporter;

  EXPECT_TRUE(exporter.export_db(ctx));

  std::filesystem::remove_all(dir);
}

TEST(UTEST_PyTorchVisionFolderExporter, crops_every_rect_into_its_tag_directory)
{
  const std::filesystem::path dir =
      std::filesystem::path{testing::TempDir()} / "utest_pytorch_crops";
  std::filesystem::remove_all(dir);
  std::filesystem::create_directories(dir);

  auto ir = ImageRecord::create("a.png", "/imgs");
  ir->rects.emplace_back(std::make_shared<ImageRecordRect>("cat", 0, 0, 5, 5));
  ir->rects.emplace_back(std::make_shared<ImageRecordRect>("dog", 5, 5, 5, 5));

  auto provider = std::make_shared<FakeProvider>();
  provider->records.emplace_back(ir);

  auto cropper = std::make_shared<FakeCropper>();

  auto ctx = std::make_shared<PyTorchExportLibraryContext>();
  ctx->set_export_path(dir.string());
  ctx->set_db_provider(provider);
  ctx->set_cropper(cropper);

  PyTorchVisionFolderExporter exporter;

  EXPECT_TRUE(exporter.export_db(ctx));

  EXPECT_TRUE(std::filesystem::is_directory(dir / "cat"));
  EXPECT_TRUE(std::filesystem::is_directory(dir / "dog"));

  ASSERT_EQ(cropper->croppedTo.size(), 2U);
  EXPECT_EQ(cropper->croppedTo[0], (dir / "cat" / "a.png").string());
  EXPECT_EQ(cropper->croppedTo[1], (dir / "dog" / "a.png").string());

  std::filesystem::remove_all(dir);
}
