#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

#include "src/exporters/Yolo42FolderExporter.h"

using namespace testing;
using iannotator::exporters::ExportContext;
using iannotator::exporters::ImageRecord;
using iannotator::exporters::ImageRecordRect;
using iannotator::exporters::ImageRecordsSet;
using iannotator::exporters::Yolo42FolderExporter;

namespace
{

class FakeProvider : public iannotator::exporters::IImagesPathsDBProvider
{
 public:
  ImageRecordsSet records;
  AnnotationsList annotations;

  ImageRecordsSet& get_images_db() override { return records; }
  std::string get_db_path() override { return {}; }
  AnnotationsList get_available_annotations() override { return annotations; }
  bool changed() override { return false; }
  bool delete_image_record(const std::string&) override { return false; }
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

void write_file(const std::filesystem::path& p, const std::string& content)
{
  std::ofstream f(p);
  f << content;
}

std::size_t count_of(const std::string& text, const std::string& part)
{
  std::size_t count{0};

  for (auto at = text.find(part); at != std::string::npos;
       at = text.find(part, at + part.size())) {
    ++count;
  }

  return count;
}

}  // namespace

TEST(UTEST_Yolo42FolderExporter, exports_the_yolo_directory_layout)
{
  const std::filesystem::path dir =
      std::filesystem::path{testing::TempDir()} / "utest_yolo_export";
  std::filesystem::remove_all(dir);

  auto provider = std::make_shared<FakeProvider>();
  provider->annotations = {"cat", "dog"};

  auto ctx = std::make_shared<ExportContext>();
  ctx->export_path = dir.string();
  ctx->dbProvider = provider;

  Yolo42FolderExporter exporter;

  EXPECT_TRUE(exporter.export_db(ctx));

  EXPECT_TRUE(std::filesystem::is_directory(dir / "data"));
  EXPECT_TRUE(std::filesystem::is_directory(dir / "cfg"));
  EXPECT_TRUE(std::filesystem::is_directory(dir / "backup"));

  ASSERT_TRUE(std::filesystem::exists(dir / "data" / "obj.names"));
  EXPECT_EQ(read_file(dir / "data" / "obj.names"), "cat\ndog");

  EXPECT_TRUE(std::filesystem::exists(dir / "data" / "obj.data"));
  EXPECT_TRUE(std::filesystem::exists(dir / "data" / "train.txt"));
  EXPECT_TRUE(std::filesystem::exists(dir / "data" / "val.txt"));

  // the descriptor is a whole darknet network, with the three detectors of
  // the YOLO v4 filled in for the two classes of the project
  ASSERT_TRUE(std::filesystem::exists(dir / "cfg" / "yolov4-obj.cfg"));

  const std::string cfg = read_file(dir / "cfg" / "yolov4-obj.cfg");

  EXPECT_NE(cfg.find("[net]"), std::string::npos);
  EXPECT_EQ(count_of(cfg, "[yolo]"), 3U);
  EXPECT_EQ(count_of(cfg, "classes=2"), 3U);
  EXPECT_EQ(count_of(cfg, "filters=21"), 3U);

  std::filesystem::remove_all(dir);
}

TEST(UTEST_Yolo42FolderExporter, normalizes_the_rectangles_of_a_copied_image)
{
  const std::filesystem::path root =
      std::filesystem::path{testing::TempDir()} / "utest_yolo_rects";
  const std::filesystem::path srcDir = root / "src";
  const std::filesystem::path dir = root / "dst";

  std::filesystem::remove_all(root);
  std::filesystem::create_directories(srcDir);

  write_file(srcDir / "a.png", "not-a-real-image");

  auto ir = ImageRecord::create("a.png", srcDir.string());
  ir->iwidth = 200;
  ir->iheight = 100;
  ir->rects.emplace_back(
      std::make_shared<ImageRecordRect>("dog", 50, 20, 100, 40));

  auto provider = std::make_shared<FakeProvider>();
  provider->annotations = {"cat", "dog"};
  provider->records.emplace_back(ir);

  auto ctx = std::make_shared<ExportContext>();
  ctx->export_path = dir.string();
  ctx->dbProvider = provider;

  Yolo42FolderExporter exporter;

  EXPECT_TRUE(exporter.export_db(ctx));

  EXPECT_TRUE(std::filesystem::is_regular_file(dir / "data" / "a.png"));
  EXPECT_EQ(read_file(dir / "data" / "train.txt"), "data/a.png");

  // class index 1 (dog is second in the sorted names), centre at
  // (50 + 100 / 2) / 200 = 0.5 and (20 + 40 / 2) / 100 = 0.4, size
  // 100 / 200 = 0.5 by 40 / 100 = 0.4
  ASSERT_TRUE(std::filesystem::exists(dir / "data" / "a.txt"));
  EXPECT_EQ(read_file(dir / "data" / "a.txt"), "1 0.5 0.4 0.5 0.4");

  std::filesystem::remove_all(root);
}
