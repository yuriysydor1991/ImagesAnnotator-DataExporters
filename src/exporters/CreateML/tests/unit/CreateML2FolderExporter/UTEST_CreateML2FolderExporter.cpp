#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

#include "src/exporters/CreateML/CreateML2FolderExporter.h"

using namespace testing;
using iannotator::exporters::CreateML2FolderExporter;
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
class UTEST_CreateML2FolderExporter : public Test
{
 public:
  void SetUp() override
  {
    root = fs::path{TempDir()} / "utest_createml";
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

  std::string annotations() { return read_file(dst / "annotations.json"); }

  fs::path root;
  fs::path srcDir;
  fs::path dst;
  std::shared_ptr<FakeProvider> provider;
  std::shared_ptr<LibraryContext> ctx;
};

}  // namespace

// Unlike every other layout of this library the images are not moved into a
// subdirectory: the data source is pointed at the one folder holding both them
// and the descriptor.
TEST_F(UTEST_CreateML2FolderExporter, exports_the_createml_layout)
{
  EXPECT_TRUE(CreateML2FolderExporter{}.export_db(ctx));

  ASSERT_TRUE(fs::is_regular_file(dst / "annotations.json"));

  EXPECT_FALSE(fs::exists(dst / "images"));

  // an empty project still leaves the one array the format is
  EXPECT_EQ(annotations(), "[\n]");
}

// The whole point of the layout: x/y is the centre of the box in the image own
// pixels, the halving every YOLO layout performs before it normalises.
TEST_F(UTEST_CreateML2FolderExporter, writes_the_pixel_centre_of_a_rectangle)
{
  with_rect("dog", 50, 20, 100, 40);

  EXPECT_TRUE(CreateML2FolderExporter{}.export_db(ctx));

  // the image sits beside the descriptor which names it
  EXPECT_TRUE(fs::is_regular_file(dst / "a.png"));

  // 50 + 100/2 = 100 across, 20 + 40/2 = 40 down
  EXPECT_EQ(annotations(),
            "[\n"
            "  {\"imagefilename\": \"a.png\", \"annotation\": [\n"
            "    {\"label\": \"dog\", \"coordinates\": {\"x\": 100, \"y\": 40, "
            "\"width\": 100, \"height\": 40}}\n"
            "  ]}\n"
            "]");
}

// A centre is an integer edge plus an integer size halved, so it lands on a
// half pixel whenever that size is odd, and the format takes a decimal there
TEST_F(UTEST_CreateML2FolderExporter, keeps_the_half_pixel_of_an_odd_size)
{
  with_rect("dog", 10, 20, 25, 41);

  EXPECT_TRUE(CreateML2FolderExporter{}.export_db(ctx));

  EXPECT_NE(annotations().find("{\"x\": 22.5, \"y\": 40.5, \"width\": 25, "
                               "\"height\": 41}"),
            std::string::npos);
}

// A centre taken from an uncut box need not lie inside the box a reader sees
TEST_F(UTEST_CreateML2FolderExporter, clamps_a_rectangle_over_the_edge)
{
  with_rect("dog", -50, -20, 100, 40);

  EXPECT_TRUE(CreateML2FolderExporter{}.export_db(ctx));

  // what is left of it is the (0, 0) to (50, 20) box, centred at (25, 10)
  EXPECT_NE(annotations().find(
                "{\"x\": 25, \"y\": 10, \"width\": 50, \"height\": 20}"),
            std::string::npos);
}

// A rectangle drawn from the right or from the bottom carries a negative size
TEST_F(UTEST_CreateML2FolderExporter, sorts_the_edges_of_a_negative_rect)
{
  with_rect("dog", 150, 60, -100, -40);

  EXPECT_TRUE(CreateML2FolderExporter{}.export_db(ctx));

  EXPECT_NE(annotations().find(
                "{\"x\": 100, \"y\": 40, \"width\": 100, \"height\": 40}"),
            std::string::npos);
}

TEST_F(UTEST_CreateML2FolderExporter, skips_a_rectangle_outside_the_image)
{
  with_rect("dog", -100, 20, 50, 40);

  EXPECT_TRUE(CreateML2FolderExporter{}.export_db(ctx));

  // the image is still exported, only the rectangle left with no area inside
  // it is dropped
  EXPECT_TRUE(fs::is_regular_file(dst / "a.png"));

  EXPECT_EQ(annotations(),
            "[\n"
            "  {\"imagefilename\": \"a.png\", \"annotation\": [\n"
            "  ]}\n"
            "]");
}

// Nothing here is numbered, so a name the available annotations list never
// heard of still reaches the descriptor as it stands
TEST_F(UTEST_CreateML2FolderExporter, writes_every_rectangle_of_an_image)
{
  auto ir = ImageRecord::create("a.png", srcDir.string());
  ir->iwidth = 200;
  ir->iheight = 100;
  ir->rects.emplace_back(
      std::make_shared<ImageRecordRect>("dog", 50, 20, 100, 40));
  ir->rects.emplace_back(
      std::make_shared<ImageRecordRect>("unlisted", 0, 0, 20, 10));

  provider->records.emplace_back(ir);

  EXPECT_TRUE(CreateML2FolderExporter{}.export_db(ctx));

  const std::string json = annotations();

  EXPECT_NE(json.find("{\"label\": \"dog\", \"coordinates\": {\"x\": 100, "
                      "\"y\": 40, \"width\": 100, \"height\": 40}},\n"),
            std::string::npos);
  EXPECT_NE(json.find("{\"label\": \"unlisted\", \"coordinates\": {\"x\": 10, "
                      "\"y\": 5, \"width\": 20, \"height\": 10}}\n"),
            std::string::npos);
}

TEST_F(UTEST_CreateML2FolderExporter, skips_the_records_it_cannot_express)
{
  auto noRects = ImageRecord::create("a.png", srcDir.string());
  noRects->iwidth = 200;
  noRects->iheight = 100;

  auto noSize = ImageRecord::create("a.png", srcDir.string());
  noSize->rects.emplace_back(
      std::make_shared<ImageRecordRect>("dog", 50, 20, 100, 40));

  provider->records.emplace_back(noRects);
  provider->records.emplace_back(noSize);

  EXPECT_TRUE(CreateML2FolderExporter{}.export_db(ctx));

  EXPECT_FALSE(fs::is_regular_file(dst / "a.png"));
  EXPECT_EQ(annotations(), "[\n]");
}

TEST_F(UTEST_CreateML2FolderExporter, gives_a_repeated_file_name_a_suffix)
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

  EXPECT_TRUE(CreateML2FolderExporter{}.export_db(ctx));

  EXPECT_TRUE(fs::is_regular_file(dst / "a.png"));
  ASSERT_TRUE(fs::is_regular_file(dst / "a-1.png"));

  // the second image is named after the file it was copied under
  EXPECT_NE(annotations().find("{\"imagefilename\": \"a-1.png\""),
            std::string::npos);
}

// The data source asks for exactly one JSON file in the directory, so the
// descriptor keeps its own name against an image which would take it
TEST_F(UTEST_CreateML2FolderExporter, keeps_the_descriptor_name_for_itself)
{
  write_file(srcDir / "annotations.json", "not-a-real-image-at-all");

  auto ir = ImageRecord::create("annotations.json", srcDir.string());
  ir->iwidth = 200;
  ir->iheight = 100;
  ir->rects.emplace_back(
      std::make_shared<ImageRecordRect>("dog", 50, 20, 100, 40));

  provider->records.emplace_back(ir);

  EXPECT_TRUE(CreateML2FolderExporter{}.export_db(ctx));

  ASSERT_TRUE(fs::is_regular_file(dst / "annotations-1.json"));

  const std::string json = annotations();

  EXPECT_NE(json.find("{\"imagefilename\": \"annotations-1.json\""),
            std::string::npos);
  EXPECT_EQ(read_file(dst / "annotations-1.json"), "not-a-real-image-at-all");
}

// An annotation name is user text, and JSON would read a quote, a backslash or
// a control character in it as syntax of its own
TEST_F(UTEST_CreateML2FolderExporter, quotes_the_names_in_the_descriptor)
{
  with_rect("a\"b", 50, 20, 100, 40);
  with_rect("c\\d", 50, 20, 100, 40);
  with_rect("e\nf", 50, 20, 100, 40);

  EXPECT_TRUE(CreateML2FolderExporter{}.export_db(ctx));

  const std::string json = annotations();

  EXPECT_NE(json.find("{\"label\": \"a\\\"b\""), std::string::npos);
  EXPECT_NE(json.find("{\"label\": \"c\\\\d\""), std::string::npos);
  EXPECT_NE(json.find("{\"label\": \"e\\u000af\""), std::string::npos);
}

TEST_F(UTEST_CreateML2FolderExporter, refuses_an_unusable_context)
{
  EXPECT_FALSE(CreateML2FolderExporter{}.export_db({}));

  auto empty = std::make_shared<LibraryContext>();

  EXPECT_FALSE(CreateML2FolderExporter{}.export_db(empty));

  empty->set_export_path(dst.string());

  EXPECT_FALSE(CreateML2FolderExporter{}.export_db(empty));
}
