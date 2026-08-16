#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

#include "src/exporters/Ultralytics/UltralyticsDetect2FolderExporter.h"
#include "src/exporters/Ultralytics/UltralyticsObb2FolderExporter.h"

using namespace testing;
using iannotator::exporters::ImageRecord;
using iannotator::exporters::ImageRecordRect;
using iannotator::exporters::ImageRecordsSet;
using iannotator::exporters::LibraryContext;
using iannotator::exporters::UltralyticsDetect2FolderExporter;
using iannotator::exporters::UltralyticsObb2FolderExporter;

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
 * @brief Drives one exporter over a single image record holding the given
 * rectangles, and hands back the export directory it has filled.
 *
 * Every case below needs the very same preparation - a source directory with
 * an image file in it, a database of one record over that file and a context
 * naming the destination - so it is written down once here.
 */
class UTEST_Ultralytics2FolderExporter : public Test
{
 public:
  void SetUp() override
  {
    root = fs::path{TempDir()} / "utest_ultralytics";
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

  std::string label_of(const std::string& stem = "a")
  {
    return read_file(dst / "labels" / "train" / (stem + ".txt"));
  }

  fs::path root;
  fs::path srcDir;
  fs::path dst;
  std::shared_ptr<FakeProvider> provider;
  std::shared_ptr<LibraryContext> ctx;
};

}  // namespace

TEST_F(UTEST_Ultralytics2FolderExporter, exports_the_ultralytics_layout)
{
  EXPECT_TRUE(UltralyticsDetect2FolderExporter{}.export_db(ctx));

  // the whole destination is the exporter's own work, the way the darknet one
  // creates its directory too
  EXPECT_TRUE(fs::is_directory(dst / "images" / "train"));
  EXPECT_TRUE(fs::is_directory(dst / "labels" / "train"));

  ASSERT_TRUE(fs::is_regular_file(dst / "data.yaml"));

  const std::string yaml = read_file(dst / "data.yaml");

  EXPECT_NE(yaml.find("path: '" + fs::absolute(dst).string() + "'"),
            std::string::npos);
  EXPECT_NE(yaml.find("\ntrain: images/train\n"), std::string::npos);
  // the whole set is offered for the validation as well, exactly as the
  // darknet layout writes one and the same list into train.txt and val.txt
  EXPECT_NE(yaml.find("\nval: images/train\n"), std::string::npos);
  EXPECT_NE(yaml.find("names:\n  0: 'cat'\n  1: 'dog'"), std::string::npos);
}

TEST_F(UTEST_Ultralytics2FolderExporter, detect_writes_the_normalized_box)
{
  with_rect("dog", 50, 20, 100, 40);

  EXPECT_TRUE(UltralyticsDetect2FolderExporter{}.export_db(ctx));

  EXPECT_TRUE(fs::is_regular_file(dst / "images" / "train" / "a.png"));

  // class index 1 (dog is second in the sorted names), centre at
  // (50 + 100 / 2) / 200 = 0.5 and (20 + 40 / 2) / 100 = 0.4, size
  // 100 / 200 = 0.5 by 40 / 100 = 0.4
  EXPECT_EQ(label_of(), "1 0.5 0.4 0.5 0.4");
}

TEST_F(UTEST_Ultralytics2FolderExporter, obb_writes_the_four_box_corners)
{
  with_rect("dog", 50, 20, 100, 40);

  EXPECT_TRUE(UltralyticsObb2FolderExporter{}.export_db(ctx));

  // the corners clockwise from the top left one: (50, 20) and (150, 60) in
  // the pixel space of the 200 by 100 image
  EXPECT_EQ(label_of(), "1 0.25 0.2 0.75 0.2 0.75 0.6 0.25 0.6");
}

// An Ultralytics release refuses the whole image over a coordinate outside of
// the 0..1 range, so the part of the rectangle that hangs over the edge is cut
// away instead of being handed over as it was drawn.
TEST_F(UTEST_Ultralytics2FolderExporter, clamps_a_rectangle_over_the_edge)
{
  with_rect("dog", -50, -20, 100, 40);

  EXPECT_TRUE(UltralyticsDetect2FolderExporter{}.export_db(ctx));

  // what is left of it is (0, 0) to (50, 20), so the centre sits at
  // 25 / 200 = 0.125 and 10 / 100 = 0.1 and the size is 0.25 by 0.2
  EXPECT_EQ(label_of(), "1 0.125 0.1 0.25 0.2");
}

// A rectangle drawn from the right or from the bottom carries a negative size
TEST_F(UTEST_Ultralytics2FolderExporter, sorts_the_edges_of_a_negative_rect)
{
  with_rect("dog", 150, 60, -100, -40);

  EXPECT_TRUE(UltralyticsObb2FolderExporter{}.export_db(ctx));

  EXPECT_EQ(label_of(), "1 0.25 0.2 0.75 0.2 0.75 0.6 0.25 0.6");
}

TEST_F(UTEST_Ultralytics2FolderExporter, skips_a_rectangle_outside_the_image)
{
  with_rect("dog", -100, 20, 50, 40);

  EXPECT_TRUE(UltralyticsDetect2FolderExporter{}.export_db(ctx));

  // the image is still exported, only the rectangle left with no area inside
  // it is dropped
  EXPECT_TRUE(fs::is_regular_file(dst / "images" / "train" / "a.png"));
  ASSERT_TRUE(fs::is_regular_file(dst / "labels" / "train" / "a.txt"));
  EXPECT_EQ(label_of(), "");
}

TEST_F(UTEST_Ultralytics2FolderExporter, skips_the_records_it_cannot_normalize)
{
  auto noRects = ImageRecord::create("a.png", srcDir.string());
  noRects->iwidth = 200;
  noRects->iheight = 100;

  auto noSize = ImageRecord::create("a.png", srcDir.string());
  noSize->rects.emplace_back(
      std::make_shared<ImageRecordRect>("dog", 50, 20, 100, 40));

  provider->records.emplace_back(noRects);
  provider->records.emplace_back(noSize);

  EXPECT_TRUE(UltralyticsDetect2FolderExporter{}.export_db(ctx));

  EXPECT_TRUE(fs::is_empty(dst / "images" / "train"));
  EXPECT_TRUE(fs::is_empty(dst / "labels" / "train"));
}

TEST_F(UTEST_Ultralytics2FolderExporter, gives_a_repeated_file_name_a_suffix)
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

  EXPECT_TRUE(UltralyticsDetect2FolderExporter{}.export_db(ctx));

  EXPECT_TRUE(fs::is_regular_file(dst / "images" / "train" / "a.png"));
  ASSERT_TRUE(fs::is_regular_file(dst / "images" / "train" / "a-1.png"));

  // the label file follows the name the image was copied under
  EXPECT_EQ(label_of("a"), "1 0.5 0.4 0.5 0.4");
  EXPECT_EQ(label_of("a-1"), "0 0.05 0.05 0.1 0.1");
}

// An annotation name is user text, and YAML would read a colon or a quote in
// it as syntax of its own
TEST_F(UTEST_Ultralytics2FolderExporter, quotes_the_names_in_the_descriptor)
{
  provider->annotations = {"a: b", "it's"};

  EXPECT_TRUE(UltralyticsDetect2FolderExporter{}.export_db(ctx));

  EXPECT_NE(
      read_file(dst / "data.yaml").find("names:\n  0: 'a: b'\n  1: 'it''s'"),
      std::string::npos);
}

TEST_F(UTEST_Ultralytics2FolderExporter, refuses_an_unusable_context)
{
  EXPECT_FALSE(UltralyticsDetect2FolderExporter{}.export_db({}));

  auto empty = std::make_shared<LibraryContext>();

  EXPECT_FALSE(UltralyticsDetect2FolderExporter{}.export_db(empty));

  empty->set_export_path(dst.string());

  EXPECT_FALSE(UltralyticsDetect2FolderExporter{}.export_db(empty));
}
