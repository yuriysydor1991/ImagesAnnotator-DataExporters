#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

#include "src/exporters/PascalVoc/PascalVoc2FolderExporter.h"

using namespace testing;
using iannotator::exporters::ImageRecord;
using iannotator::exporters::ImageRecordRect;
using iannotator::exporters::ImageRecordsSet;
using iannotator::exporters::LibraryContext;
using iannotator::exporters::PascalVoc2FolderExporter;

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
class UTEST_PascalVoc2FolderExporter : public Test
{
 public:
  void SetUp() override
  {
    root = fs::path{TempDir()} / "utest_pascal_voc";
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

  std::string annotation(const std::string& stem = "a")
  {
    return read_file(dst / "Annotations" / (stem + ".xml"));
  }

  /// @brief The training list, which the validation one always repeats
  std::string image_set()
  {
    const fs::path setsDir = dst / "ImageSets" / "Main";
    const std::string train = read_file(setsDir / "train.txt");

    EXPECT_EQ(read_file(setsDir / "val.txt"), train);

    return train;
  }

  fs::path root;
  fs::path srcDir;
  fs::path dst;
  std::shared_ptr<FakeProvider> provider;
  std::shared_ptr<LibraryContext> ctx;
};

}  // namespace

TEST_F(UTEST_PascalVoc2FolderExporter, exports_the_pascal_voc_layout)
{
  EXPECT_TRUE(PascalVoc2FolderExporter{}.export_db(ctx));

  // the whole destination is the exporter's own work, the way the darknet, the
  // Ultralytics and the COCO ones create their directories too
  EXPECT_TRUE(fs::is_directory(dst / "JPEGImages"));
  EXPECT_TRUE(fs::is_directory(dst / "Annotations"));

  // the lists a devkit reader is pointed at exist even with nothing in them,
  // and they are named after the splits such a reader asks for
  ASSERT_TRUE(fs::is_regular_file(dst / "ImageSets" / "Main" / "train.txt"));
  ASSERT_TRUE(fs::is_regular_file(dst / "ImageSets" / "Main" / "val.txt"));
  EXPECT_TRUE(image_set().empty());
}

// The whole point of the layout: the rectangle reaches the descriptor as the
// two corner points it was drawn between, in the image own pixels, which is
// the pair LabelImg turns back into the very same rectangle
TEST_F(UTEST_PascalVoc2FolderExporter, writes_the_corner_bndbox_of_a_rectangle)
{
  with_rect("dog", 50, 20, 100, 40);

  EXPECT_TRUE(PascalVoc2FolderExporter{}.export_db(ctx));

  EXPECT_TRUE(fs::is_regular_file(dst / "JPEGImages" / "a.png"));
  EXPECT_EQ(image_set(), "a");

  EXPECT_EQ(annotation(),
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<annotation>\n"
            "  <folder>JPEGImages</folder>\n"
            "  <filename>a.png</filename>\n"
            "  <source>\n"
            "    <database>The ImagesAnnotator annotations dataset</database>\n"
            "  </source>\n"
            "  <size>\n"
            "    <width>200</width>\n"
            "    <height>100</height>\n"
            "    <depth>3</depth>\n"
            "  </size>\n"
            "  <segmented>0</segmented>\n"
            "  <object>\n"
            "    <name>dog</name>\n"
            "    <pose>Unspecified</pose>\n"
            "    <truncated>0</truncated>\n"
            "    <difficult>0</difficult>\n"
            "    <bndbox>\n"
            "      <xmin>50</xmin>\n"
            "      <ymin>20</ymin>\n"
            "      <xmax>150</xmax>\n"
            "      <ymax>60</ymax>\n"
            "    </bndbox>\n"
            "  </object>\n"
            "</annotation>");
}

// A box reaching over an image edge would otherwise name pixels the image
// does not have, and what the flag next to it says is that the object really
// does continue past that edge
TEST_F(UTEST_PascalVoc2FolderExporter, clamps_a_rectangle_over_the_edge)
{
  with_rect("dog", -50, -20, 100, 40);

  EXPECT_TRUE(PascalVoc2FolderExporter{}.export_db(ctx));

  const std::string xml = annotation();

  // what is left of it is the (0, 0) to (50, 20) box
  EXPECT_NE(xml.find("      <xmin>0</xmin>\n"
                     "      <ymin>0</ymin>\n"
                     "      <xmax>50</xmax>\n"
                     "      <ymax>20</ymax>\n"),
            std::string::npos);
  EXPECT_NE(xml.find("<truncated>1</truncated>"), std::string::npos);
}

// A rectangle drawn from the right or from the bottom carries a negative size
TEST_F(UTEST_PascalVoc2FolderExporter, sorts_the_edges_of_a_negative_rect)
{
  with_rect("dog", 150, 60, -100, -40);

  EXPECT_TRUE(PascalVoc2FolderExporter{}.export_db(ctx));

  const std::string xml = annotation();

  EXPECT_NE(xml.find("      <xmin>50</xmin>\n"
                     "      <ymin>20</ymin>\n"
                     "      <xmax>150</xmax>\n"
                     "      <ymax>60</ymax>\n"),
            std::string::npos);
  // sorting the edges is not cutting them: nothing of this one was lost
  EXPECT_NE(xml.find("<truncated>0</truncated>"), std::string::npos);
}

TEST_F(UTEST_PascalVoc2FolderExporter, skips_a_rectangle_outside_the_image)
{
  with_rect("dog", -100, 20, 50, 40);

  EXPECT_TRUE(PascalVoc2FolderExporter{}.export_db(ctx));

  // the image is still exported, only the rectangle left with no area inside
  // it is dropped
  EXPECT_TRUE(fs::is_regular_file(dst / "JPEGImages" / "a.png"));
  EXPECT_EQ(image_set(), "a");

  const std::string xml = annotation();

  EXPECT_NE(xml.find("<filename>a.png</filename>"), std::string::npos);
  EXPECT_EQ(xml.find("<object>"), std::string::npos);
}

TEST_F(UTEST_PascalVoc2FolderExporter, skips_the_records_it_cannot_express)
{
  auto noRects = ImageRecord::create("a.png", srcDir.string());
  noRects->iwidth = 200;
  noRects->iheight = 100;

  auto noSize = ImageRecord::create("a.png", srcDir.string());
  noSize->rects.emplace_back(
      std::make_shared<ImageRecordRect>("dog", 50, 20, 100, 40));

  provider->records.emplace_back(noRects);
  provider->records.emplace_back(noSize);

  EXPECT_TRUE(PascalVoc2FolderExporter{}.export_db(ctx));

  EXPECT_TRUE(fs::is_empty(dst / "JPEGImages"));
  EXPECT_TRUE(fs::is_empty(dst / "Annotations"));
  EXPECT_TRUE(image_set().empty());
}

// The descriptor of an image is the file next to it named by its very stem, so
// a name another extension already took collides just as the whole file name
// does
TEST_F(UTEST_PascalVoc2FolderExporter, gives_a_repeated_file_stem_a_suffix)
{
  const fs::path otherDir = root / "other";
  fs::create_directories(otherDir);
  write_file(otherDir / "a.png", "not-a-real-image-either");
  write_file(srcDir / "a.jpg", "not-a-real-image-of-another-kind");

  with_rect("dog", 50, 20, 100, 40);

  auto sameName = ImageRecord::create("a.png", otherDir.string());
  sameName->iwidth = 200;
  sameName->iheight = 100;
  sameName->rects.emplace_back(
      std::make_shared<ImageRecordRect>("cat", 0, 0, 20, 10));

  auto sameStem = ImageRecord::create("a.jpg", srcDir.string());
  sameStem->iwidth = 200;
  sameStem->iheight = 100;
  sameStem->rects.emplace_back(
      std::make_shared<ImageRecordRect>("cat", 10, 10, 20, 10));

  provider->records.emplace_back(sameName);
  provider->records.emplace_back(sameStem);

  EXPECT_TRUE(PascalVoc2FolderExporter{}.export_db(ctx));

  EXPECT_TRUE(fs::is_regular_file(dst / "JPEGImages" / "a.png"));
  EXPECT_TRUE(fs::is_regular_file(dst / "JPEGImages" / "a-1.png"));
  ASSERT_TRUE(fs::is_regular_file(dst / "JPEGImages" / "a-2.jpg"));

  EXPECT_EQ(image_set(), "a\na-1\na-2");

  // every descriptor names the file its own image was copied under
  EXPECT_NE(annotation("a-1").find("<filename>a-1.png</filename>"),
            std::string::npos);
  EXPECT_NE(annotation("a-2").find("<filename>a-2.jpg</filename>"),
            std::string::npos);
}

// An annotation name is user text, and XML would read the markup characters in
// it as syntax of its own
TEST_F(UTEST_PascalVoc2FolderExporter, escapes_the_names_in_the_descriptor)
{
  with_rect("a<b>&c", 50, 20, 100, 40);

  EXPECT_TRUE(PascalVoc2FolderExporter{}.export_db(ctx));

  EXPECT_NE(annotation().find("<name>a&lt;b&gt;&amp;c</name>"),
            std::string::npos);
}

// XML 1.0 carries no way at all to write these, not even a numeric reference
TEST_F(UTEST_PascalVoc2FolderExporter, drops_the_characters_xml_forbids)
{
  with_rect(std::string{"a\x01"} + "b\tc", 50, 20, 100, 40);

  EXPECT_TRUE(PascalVoc2FolderExporter{}.export_db(ctx));

  EXPECT_NE(annotation().find("<name>ab\tc</name>"), std::string::npos);
}

TEST_F(UTEST_PascalVoc2FolderExporter, refuses_an_unusable_context)
{
  EXPECT_FALSE(PascalVoc2FolderExporter{}.export_db({}));

  auto empty = std::make_shared<LibraryContext>();

  EXPECT_FALSE(PascalVoc2FolderExporter{}.export_db(empty));

  empty->set_export_path(dst.string());

  EXPECT_FALSE(PascalVoc2FolderExporter{}.export_db(empty));
}
