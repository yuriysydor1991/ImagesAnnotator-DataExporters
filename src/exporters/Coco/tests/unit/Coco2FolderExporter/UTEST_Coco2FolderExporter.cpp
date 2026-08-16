#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

#include "src/exporters/Coco/Coco2FolderExporter.h"

using namespace testing;
using iannotator::exporters::Coco2FolderExporter;
using iannotator::exporters::ImageRecord;
using iannotator::exporters::ImageRecordRect;
using iannotator::exporters::ImageRecordsSet;
using iannotator::exporters::LibraryContext;

namespace
{

namespace fs = std::filesystem;

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

std::string read_file(const fs::path& p)
{
  std::ifstream f(p);
  std::stringstream ss;
  ss << f.rdbuf();
  std::string s = ss.str();
  while (!s.empty() && (s.back() == '\n' || s.back() == '\r')) s.pop_back();
  return s;
}

void write_file(const fs::path& p, const std::string& content)
{
  std::ofstream f(p);
  f << content;
}

/**
 * @brief Drives the exporter over a single image record holding the given
 * rectangles, and hands back the descriptor it has written.
 *
 * Every case below needs the very same preparation - a source directory with
 * an image file in it, a database of one record over that file and a context
 * naming the destination - so it is written down once here.
 */
class UTEST_Coco2FolderExporter : public Test
{
 public:
  void SetUp() override
  {
    root = fs::path{TempDir()} / "utest_coco";
    srcDir = root / "src";
    dst = root / "dst";

    fs::remove_all(root);
    fs::create_directories(srcDir);

    write_file(srcDir / "a.png", "not-a-real-image");

    provider = std::make_shared<FakeProvider>();
    provider->annotations = {"cat", "dog"};

    ctx = std::make_shared<LibraryContext>();
    ctx->set_export_path(dst.string());
    ctx->set_db_provider(provider);
  }

  void TearDown() override { fs::remove_all(root); }

  /// @brief Puts one record of the given rectangle into the database
  void with_rect(const std::string& name, int x, int y, int width, int height)
  {
    auto ir = ImageRecord::create("a.png", srcDir.string());
    ir->iwidth = 200;
    ir->iheight = 100;
    ir->rects.emplace_back(
        std::make_shared<ImageRecordRect>(name, x, y, width, height));

    provider->records.emplace_back(ir);
  }

  std::string instances()
  {
    return read_file(dst / "annotations" / "instances_default.json");
  }

  fs::path root;
  fs::path srcDir;
  fs::path dst;
  std::shared_ptr<FakeProvider> provider;
  std::shared_ptr<LibraryContext> ctx;
};

}  // namespace

TEST_F(UTEST_Coco2FolderExporter, exports_the_coco_layout)
{
  EXPECT_TRUE(Coco2FolderExporter{}.export_db(ctx));

  // the whole destination is the exporter's own work, the way the darknet and
  // the Ultralytics ones create their directories too
  EXPECT_TRUE(fs::is_directory(dst / "images"));
  ASSERT_TRUE(
      fs::is_regular_file(dst / "annotations" / "instances_default.json"));

  const std::string json = instances();

  EXPECT_NE(json.find("\"info\": {\"description\": \"The ImagesAnnotator "
                      "annotations dataset\", \"version\": \""),
            std::string::npos);
  EXPECT_NE(json.find("\"licenses\": [],"), std::string::npos);
  EXPECT_NE(json.find("\"images\": [\n],"), std::string::npos);
  EXPECT_NE(json.find("\"annotations\": [\n],"), std::string::npos);
  EXPECT_NE(json.find("\"categories\": [\n"
                      "  {\"id\": 1, \"name\": \"cat\", \"supercategory\": "
                      "\"\"},\n"
                      "  {\"id\": 2, \"name\": \"dog\", \"supercategory\": "
                      "\"\"}\n"
                      "]"),
            std::string::npos);
}

// The whole point of the layout: the four numbers of the rectangle reach the
// bbox as the image own pixels they were drawn in, with nothing normalised
// away the way every YOLO layout normalises it
TEST_F(UTEST_Coco2FolderExporter, writes_the_pixel_bbox_of_a_rectangle)
{
  with_rect("dog", 50, 20, 100, 40);

  EXPECT_TRUE(Coco2FolderExporter{}.export_db(ctx));

  EXPECT_TRUE(fs::is_regular_file(dst / "images" / "a.png"));

  const std::string json = instances();

  // the file name alone, since the images directory itself is what a reader is
  // given as the dataset root
  EXPECT_NE(json.find("\"images\": [\n"
                      "  {\"id\": 1, \"file_name\": \"a.png\", \"width\": 200, "
                      "\"height\": 100}\n"
                      "]"),
            std::string::npos);

  // the category 2 is the dog, the second name of the sorted list, since a
  // reader treats the category 0 as the background one
  EXPECT_NE(json.find("\"annotations\": [\n"
                      "  {\"id\": 1, \"image_id\": 1, \"category_id\": 2, "
                      "\"bbox\": [50, 20, 100, 40], \"area\": 4000, "
                      "\"iscrowd\": 0, \"segmentation\": []}\n"
                      "]"),
            std::string::npos);
}

// The area written next to a box reaching over an image edge, and every
// overlap ever computed against it, would otherwise be a lie
TEST_F(UTEST_Coco2FolderExporter, clamps_a_rectangle_over_the_edge)
{
  with_rect("dog", -50, -20, 100, 40);

  EXPECT_TRUE(Coco2FolderExporter{}.export_db(ctx));

  // what is left of it is the (0, 0) to (50, 20) box
  EXPECT_NE(instances().find("\"bbox\": [0, 0, 50, 20], \"area\": 1000"),
            std::string::npos);
}

// A rectangle drawn from the right or from the bottom carries a negative size
TEST_F(UTEST_Coco2FolderExporter, sorts_the_edges_of_a_negative_rect)
{
  with_rect("dog", 150, 60, -100, -40);

  EXPECT_TRUE(Coco2FolderExporter{}.export_db(ctx));

  EXPECT_NE(instances().find("\"bbox\": [50, 20, 100, 40], \"area\": 4000"),
            std::string::npos);
}

TEST_F(UTEST_Coco2FolderExporter, skips_a_rectangle_outside_the_image)
{
  with_rect("dog", -100, 20, 50, 40);

  EXPECT_TRUE(Coco2FolderExporter{}.export_db(ctx));

  // the image is still exported, only the rectangle left with no area inside
  // it is dropped
  EXPECT_TRUE(fs::is_regular_file(dst / "images" / "a.png"));

  const std::string json = instances();

  EXPECT_NE(json.find("{\"id\": 1, \"file_name\": \"a.png\""),
            std::string::npos);
  EXPECT_NE(json.find("\"annotations\": [\n],"), std::string::npos);
}

TEST_F(UTEST_Coco2FolderExporter, skips_the_records_it_cannot_express)
{
  auto noRects = ImageRecord::create("a.png", srcDir.string());
  noRects->iwidth = 200;
  noRects->iheight = 100;

  auto noSize = ImageRecord::create("a.png", srcDir.string());
  noSize->rects.emplace_back(
      std::make_shared<ImageRecordRect>("dog", 50, 20, 100, 40));

  provider->records.emplace_back(noRects);
  provider->records.emplace_back(noSize);

  EXPECT_TRUE(Coco2FolderExporter{}.export_db(ctx));

  EXPECT_TRUE(fs::is_empty(dst / "images"));

  const std::string json = instances();

  EXPECT_NE(json.find("\"images\": [\n],"), std::string::npos);
  EXPECT_NE(json.find("\"annotations\": [\n],"), std::string::npos);
}

TEST_F(UTEST_Coco2FolderExporter, gives_a_repeated_file_name_a_suffix)
{
  const fs::path otherDir = root / "other";
  fs::create_directories(otherDir);
  write_file(otherDir / "a.png", "not-a-real-image-either");

  with_rect("dog", 50, 20, 100, 40);

  auto other = ImageRecord::create("a.png", otherDir.string());
  other->iwidth = 200;
  other->iheight = 100;
  other->rects.emplace_back(
      std::make_shared<ImageRecordRect>("cat", 0, 0, 20, 10));

  provider->records.emplace_back(other);

  EXPECT_TRUE(Coco2FolderExporter{}.export_db(ctx));

  EXPECT_TRUE(fs::is_regular_file(dst / "images" / "a.png"));
  ASSERT_TRUE(fs::is_regular_file(dst / "images" / "a-1.png"));

  const std::string json = instances();

  // the second image is named after the file it was copied under, and every
  // identifier of the descriptor keeps counting over it
  EXPECT_NE(json.find("  {\"id\": 2, \"file_name\": \"a-1.png\", \"width\": "
                      "200, \"height\": 100}\n"),
            std::string::npos);
  EXPECT_NE(json.find("  {\"id\": 2, \"image_id\": 2, \"category_id\": 1, "
                      "\"bbox\": [0, 0, 20, 10], \"area\": 200,"),
            std::string::npos);
}

// An annotation name is user text, and JSON would read a quote, a backslash or
// a control character in it as syntax of its own
TEST_F(UTEST_Coco2FolderExporter, quotes_the_names_in_the_descriptor)
{
  provider->annotations = {"a\"b", "c\\d", "e\nf"};

  EXPECT_TRUE(Coco2FolderExporter{}.export_db(ctx));

  const std::string json = instances();

  EXPECT_NE(json.find("{\"id\": 1, \"name\": \"a\\\"b\""), std::string::npos);
  EXPECT_NE(json.find("{\"id\": 2, \"name\": \"c\\\\d\""), std::string::npos);
  EXPECT_NE(json.find("{\"id\": 3, \"name\": \"e\\u000af\""),
            std::string::npos);
}

TEST_F(UTEST_Coco2FolderExporter, refuses_an_unusable_context)
{
  EXPECT_FALSE(Coco2FolderExporter{}.export_db({}));

  auto empty = std::make_shared<LibraryContext>();

  EXPECT_FALSE(Coco2FolderExporter{}.export_db(empty));

  empty->set_export_path(dst.string());

  EXPECT_FALSE(Coco2FolderExporter{}.export_db(empty));
}
