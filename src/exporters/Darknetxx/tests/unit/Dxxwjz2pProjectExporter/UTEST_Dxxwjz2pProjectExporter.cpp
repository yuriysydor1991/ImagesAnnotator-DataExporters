#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <zlib.h>

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "src/exporters/Darknetxx/Dxxwjz2pAnnotationsWriter.h"
#include "src/exporters/Darknetxx/Dxxwjz2pProjectExporter.h"
#include "src/exporters/Darknetxx/Dxxwjz2pProjectWriter.h"
#include "src/exporters/Yolo4/Yolov4CfgWriter.h"

using namespace testing;
using iannotator::exporters::Dxxwjz2pAnnotationsWriter;
using iannotator::exporters::Dxxwjz2pProjectExporter;
using iannotator::exporters::Dxxwjz2pProjectWriter;
using iannotator::exporters::IADataExportersContext;
using iannotator::exporters::ImageRecord;
using iannotator::exporters::ImageRecordPtr;
using iannotator::exporters::ImageRecordRect;
using iannotator::exporters::ImageRecordsSet;
using iannotator::exporters::Yolov4CfgWriter;

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

std::string read_binary(const std::filesystem::path& p)
{
  std::ifstream f(p, std::ios::binary);

  return {std::istreambuf_iterator<char>{f}, std::istreambuf_iterator<char>{}};
}

void write_file(const std::filesystem::path& p, const std::string& content)
{
  std::ofstream f(p);
  f << content;
}

/**
 * @brief The text of the gzip file of one member, decompressed by the zlib,
 * or an empty text for anything else.
 */
std::string gunzip(const std::string& member)
{
  // the window bits of the gzip wrapper
  constexpr int gzipWindowBits{15 + 16};

  z_stream zs{};

  if (inflateInit2(&zs, gzipWindowBits) != Z_OK) {
    return {};
  }

  zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(member.data()));
  zs.avail_in = static_cast<uInt>(member.size());

  std::string text;
  std::vector<char> chunk(16384U);
  int ret{Z_OK};

  do {
    zs.next_out = reinterpret_cast<Bytef*>(chunk.data());
    zs.avail_out = static_cast<uInt>(chunk.size());

    ret = inflate(&zs, Z_NO_FLUSH);

    text.append(chunk.data(), chunk.size() - zs.avail_out);
  } while (ret == Z_OK);

  const bool whole = ret == Z_STREAM_END && zs.avail_in == 0U;

  inflateEnd(&zs);

  return whole ? text : std::string{};
}

/// @brief The project the exporter has to write of the class names and of the
/// images of the images folder given
std::string expected_project(const std::vector<std::string>& classes,
                             const Dxxwjz2pAnnotationsWriter::Images& images)
{
  std::ostringstream cfg;

  Yolov4CfgWriter{cfg, classes.size(), images.size(),
                  "darknetxx --train-cfg yolov4-obj.dxxwjz2p"}
      .write();

  std::ostringstream document;

  EXPECT_TRUE(
      (Dxxwjz2pProjectWriter{document, cfg.str(), classes, "images", images}
           .write()));

  return document.str();
}

ImageRecordPtr image_of(const std::filesystem::path& dir,
                        const std::string& name, int width, int height)
{
  write_file(dir / name, "image " + (dir / name).string());

  auto ir = ImageRecord::create(name, dir.string());
  ir->iwidth = width;
  ir->iheight = height;
  return ir;
}

std::shared_ptr<IADataExportersContext> context_of(
    const std::filesystem::path& dir, std::shared_ptr<FakeProvider> provider)
{
  auto ctx = std::make_shared<IADataExportersContext>();
  ctx->set_export_path(dir.string());
  ctx->set_db_provider(provider);
  return ctx;
}

}  // namespace

TEST(UTEST_Dxxwjz2pProjectExporter, writes_the_project_beside_the_images)
{
  const std::filesystem::path root =
      std::filesystem::path{testing::TempDir()} / "utest_dxxwjz2p_export";
  const std::filesystem::path srcDir = root / "src";
  const std::filesystem::path dir = root / "dst";

  std::filesystem::remove_all(root);
  std::filesystem::create_directories(srcDir);

  auto ir = image_of(srcDir, "a.png", 200, 100);
  ir->rects.emplace_back(
      std::make_shared<ImageRecordRect>("dog", 50, 20, 100, 40));

  auto provider = std::make_shared<FakeProvider>();
  provider->annotations = {"cat", "dog"};
  provider->records.emplace_back(ir);

  ASSERT_TRUE(Dxxwjz2pProjectExporter{}.export_db(context_of(dir, provider)));

  EXPECT_TRUE(std::filesystem::is_directory(dir / "backup"));
  EXPECT_EQ(read_binary(dir / "images" / "a.png"),
            read_binary(srcDir / "a.png"));

  const std::filesystem::path project = dir / "yolov4-obj.dxxwjz2p";

  ASSERT_TRUE(std::filesystem::is_regular_file(project));
  EXPECT_EQ(gunzip(read_binary(project)),
            expected_project({"cat", "dog"}, {{"a.png", ir}}));

  // the project alone: neither the YOLO v4 directory nor the other project
  EXPECT_FALSE(std::filesystem::exists(dir / "data"));
  EXPECT_FALSE(std::filesystem::exists(dir / "cfg"));
  EXPECT_FALSE(std::filesystem::exists(dir / "yolov4-obj.dxxwjz3p"));

  std::filesystem::remove_all(root);
}

// the images of the YOLO v4 export: the ones of the boxes and of a known size
TEST(UTEST_Dxxwjz2pProjectExporter,
     keeps_the_images_of_the_boxes_and_of_the_size)
{
  const std::filesystem::path root =
      std::filesystem::path{testing::TempDir()} / "utest_dxxwjz2p_skip";
  const std::filesystem::path srcDir = root / "src";
  const std::filesystem::path dir = root / "dst";

  std::filesystem::remove_all(root);
  std::filesystem::create_directories(srcDir);

  auto boxed = image_of(srcDir, "boxed.png", 20, 10);
  boxed->rects.emplace_back(
      std::make_shared<ImageRecordRect>("cat", 1, 2, 3, 4));

  auto unboxed = image_of(srcDir, "unboxed.png", 20, 10);

  auto unsized = image_of(srcDir, "unsized.png", 0, 10);
  unsized->rects.emplace_back(
      std::make_shared<ImageRecordRect>("cat", 1, 2, 3, 4));

  auto missing = ImageRecord::create("missing.png", srcDir.string());
  missing->iwidth = 20;
  missing->iheight = 10;
  missing->rects.emplace_back(
      std::make_shared<ImageRecordRect>("cat", 1, 2, 3, 4));

  auto provider = std::make_shared<FakeProvider>();
  provider->annotations = {"cat"};
  provider->records = {unboxed, boxed, unsized, missing};

  ASSERT_TRUE(Dxxwjz2pProjectExporter{}.export_db(context_of(dir, provider)));

  EXPECT_TRUE(std::filesystem::is_regular_file(dir / "images" / "boxed.png"));
  EXPECT_FALSE(std::filesystem::exists(dir / "images" / "unboxed.png"));
  EXPECT_FALSE(std::filesystem::exists(dir / "images" / "unsized.png"));
  EXPECT_FALSE(std::filesystem::exists(dir / "images" / "missing.png"));

  EXPECT_EQ(gunzip(read_binary(dir / "yolov4-obj.dxxwjz2p")),
            expected_project({"cat"}, {{"boxed.png", boxed}}));

  std::filesystem::remove_all(root);
}

// the images of the same name of the different folders go into one folder
TEST(UTEST_Dxxwjz2pProjectExporter, names_the_copies_of_the_same_name_apart)
{
  const std::filesystem::path root =
      std::filesystem::path{testing::TempDir()} / "utest_dxxwjz2p_names";
  const std::filesystem::path dir = root / "dst";

  std::filesystem::remove_all(root);

  ImageRecordsSet records;
  Dxxwjz2pAnnotationsWriter::Images expected;

  for (const std::string folder : {"x", "y", "z"}) {
    std::filesystem::create_directories(root / folder);

    auto ir = image_of(root / folder, "a.png", 20, 10);
    ir->rects.emplace_back(
        std::make_shared<ImageRecordRect>(folder, 1, 2, 3, 4));

    records.push_back(ir);
  }

  expected = {
      {"a.png", records[0]}, {"a-1.png", records[1]}, {"a-2.png", records[2]}};

  auto provider = std::make_shared<FakeProvider>();
  provider->annotations = {"x", "y", "z"};
  provider->records = records;

  ASSERT_TRUE(Dxxwjz2pProjectExporter{}.export_db(context_of(dir, provider)));

  for (std::size_t index = 0; index < expected.size(); ++index) {
    EXPECT_EQ(read_binary(dir / "images" / expected[index].relPath),
              read_binary(records[index]->get_full_path()))
        << expected[index].relPath;
  }

  EXPECT_EQ(gunzip(read_binary(dir / "yolov4-obj.dxxwjz2p")),
            expected_project({"x", "y", "z"}, expected));

  std::filesystem::remove_all(root);
}

// the darknetxx trains no detector of no classes, but the export of an empty
// project still describes the network
TEST(UTEST_Dxxwjz2pProjectExporter, writes_the_project_of_no_image_too)
{
  const std::filesystem::path dir =
      std::filesystem::path{testing::TempDir()} / "utest_dxxwjz2p_empty";
  std::filesystem::remove_all(dir);

  auto provider = std::make_shared<FakeProvider>();

  ASSERT_TRUE(Dxxwjz2pProjectExporter{}.export_db(context_of(dir, provider)));

  const std::string document = gunzip(read_binary(dir / "yolov4-obj.dxxwjz2p"));

  EXPECT_EQ(document, expected_project({}, {}));
  EXPECT_NE(document.find("\"annotations\": []}\n}\n"), std::string::npos);

  std::filesystem::remove_all(dir);
}

TEST(UTEST_Dxxwjz2pProjectExporter,
     writes_no_project_where_no_directory_is_made)
{
  const std::filesystem::path root =
      std::filesystem::path{testing::TempDir()} / "utest_dxxwjz2p_blocked";
  std::filesystem::remove_all(root);
  std::filesystem::create_directories(root);

  // a regular file takes the place of the export directory
  const std::filesystem::path dir = root / "dst";
  write_file(dir, "a file, not a directory");

  auto provider = std::make_shared<FakeProvider>();
  provider->annotations = {"cat"};

  EXPECT_FALSE(Dxxwjz2pProjectExporter{}.export_db(context_of(dir, provider)));
  EXPECT_TRUE(std::filesystem::is_regular_file(dir));

  std::filesystem::remove_all(root);
}
