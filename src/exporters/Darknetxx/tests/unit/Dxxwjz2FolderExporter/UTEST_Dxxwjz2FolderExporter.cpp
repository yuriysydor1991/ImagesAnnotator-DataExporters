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

#include "src/exporters/Darknetxx/Dxxwjz2FolderExporter.h"
#include "src/exporters/Darknetxx/Dxxwjz2NetworkWriter.h"
#include "src/exporters/Yolo4/Yolo42FolderExporter.h"

using namespace testing;
using iannotator::exporters::Dxxwjz2FolderExporter;
using iannotator::exporters::Dxxwjz2NetworkWriter;
using iannotator::exporters::IADataExportersContext;
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

std::string read_binary(const std::filesystem::path& p)
{
  std::ifstream f(p, std::ios::binary);

  return {std::istreambuf_iterator<char>{f}, std::istreambuf_iterator<char>{}};
}

std::string read_file(const std::filesystem::path& p)
{
  std::string s = read_binary(p);
  while (!s.empty() && (s.back() == '\n' || s.back() == '\r')) s.pop_back();
  return s;
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

/// @brief The document the exporter has to write for the cfg file of the
/// export and the class names given
std::string expected_network(const std::filesystem::path& dir,
                             const std::vector<std::string>& classes)
{
  std::ostringstream document;

  const std::string cfg = read_binary(dir / "cfg" / "yolov4-obj.cfg");

  Dxxwjz2NetworkWriter{document, cfg, classes}.write();

  return document.str();
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

TEST(UTEST_Dxxwjz2FolderExporter, adds_the_network_to_train_to_the_yolo_layout)
{
  const std::filesystem::path dir =
      std::filesystem::path{testing::TempDir()} / "utest_dxxwjz2_export";
  std::filesystem::remove_all(dir);

  auto provider = std::make_shared<FakeProvider>();
  provider->annotations = {"cat", "dog"};

  Dxxwjz2FolderExporter exporter;

  ASSERT_TRUE(exporter.export_db(context_of(dir, provider)));

  // the directory the darknetxx-gui trains
  EXPECT_TRUE(std::filesystem::is_directory(dir / "backup"));
  EXPECT_EQ(read_file(dir / "data" / "obj.names"), "cat\ndog");
  EXPECT_TRUE(std::filesystem::is_regular_file(dir / "data" / "train.txt"));
  ASSERT_TRUE(std::filesystem::is_regular_file(dir / "cfg" / "yolov4-obj.cfg"));

  // and the network to train beside the cfg, carrying the cfg text as it was
  // written into the file
  const std::filesystem::path network = dir / "cfg" / "yolov4-obj.dxxwjz2";

  ASSERT_TRUE(std::filesystem::is_regular_file(network));

  const std::string document = gunzip(read_binary(network));

  EXPECT_EQ(document, expected_network(dir, {"cat", "dog"}));
  EXPECT_EQ(
      document.rfind("{\n  \"format\": \"dxxwjz2\",\n  \"version\": 2,\n", 0),
      0U);
  EXPECT_NE(document.find("\"classes\": [\n"
                          "    {\"index\": 0, \"name\": \"cat\"},\n"
                          "    {\"index\": 1, \"name\": \"dog\"}\n  ]}"),
            std::string::npos);

  std::filesystem::remove_all(dir);
}

// the class index of a name has to be the one the label files number it by,
// or the network learns every class under the name of another one
TEST(UTEST_Dxxwjz2FolderExporter, numbers_the_classes_the_way_the_labels_do)
{
  const std::filesystem::path root =
      std::filesystem::path{testing::TempDir()} / "utest_dxxwjz2_rects";
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
  provider->annotations = {"dog", "cat", "zebra"};
  provider->records.emplace_back(ir);

  Dxxwjz2FolderExporter exporter;

  ASSERT_TRUE(exporter.export_db(context_of(dir, provider)));

  // the dog is the second of the sorted names in the labels
  EXPECT_EQ(read_file(dir / "data" / "a.txt"), "1 0.5 0.4 0.5 0.4");

  const std::string document =
      gunzip(read_binary(dir / "cfg" / "yolov4-obj.dxxwjz2"));

  EXPECT_EQ(document, expected_network(dir, {"cat", "dog", "zebra"}));
  EXPECT_NE(document.find("{\"index\": 1, \"name\": \"dog\"}"),
            std::string::npos);

  std::filesystem::remove_all(root);
}

// everything but the network to train is the YOLO v4 export byte for byte,
// which is what keeps the directory trainable by the darknetxx-gui
TEST(UTEST_Dxxwjz2FolderExporter, writes_the_yolo_files_the_yolo4_export_does)
{
  const std::filesystem::path root =
      std::filesystem::path{testing::TempDir()} / "utest_dxxwjz2_parity";
  const std::filesystem::path srcDir = root / "src";

  std::filesystem::remove_all(root);
  std::filesystem::create_directories(srcDir);

  write_file(srcDir / "a.png", "not-a-real-image");

  auto ir = ImageRecord::create("a.png", srcDir.string());
  ir->iwidth = 640;
  ir->iheight = 480;
  ir->rects.emplace_back(
      std::make_shared<ImageRecordRect>("cat", 10, 20, 30, 40));

  auto provider = std::make_shared<FakeProvider>();
  provider->annotations = {"cat"};
  provider->records.emplace_back(ir);

  Yolo42FolderExporter yolo;
  Dxxwjz2FolderExporter darknetxx;

  ASSERT_TRUE(yolo.export_db(context_of(root / "yolo", provider)));
  ASSERT_TRUE(darknetxx.export_db(context_of(root / "darknetxx", provider)));

  std::size_t compared{0};

  for (const auto& entry :
       std::filesystem::recursive_directory_iterator{root / "yolo"}) {
    if (!entry.is_regular_file()) {
      continue;
    }

    const auto rel = std::filesystem::relative(entry.path(), root / "yolo");

    EXPECT_EQ(read_binary(root / "darknetxx" / rel), read_binary(entry.path()))
        << rel;

    ++compared;
  }

  // the network descriptor, the names, the data, the two lists, the image and
  // its labels
  EXPECT_EQ(compared, 7U);
  EXPECT_TRUE(std::filesystem::is_regular_file(root / "darknetxx" / "cfg" /
                                               "yolov4-obj.dxxwjz2"));

  std::filesystem::remove_all(root);
}

TEST(UTEST_Dxxwjz2FolderExporter, replaces_the_network_of_a_previous_export)
{
  const std::filesystem::path dir =
      std::filesystem::path{testing::TempDir()} / "utest_dxxwjz2_again";
  std::filesystem::remove_all(dir);

  auto provider = std::make_shared<FakeProvider>();
  provider->annotations = {"cat", "dog", "owl"};

  ASSERT_TRUE(Dxxwjz2FolderExporter{}.export_db(context_of(dir, provider)));

  provider->annotations = {"cat"};

  ASSERT_TRUE(Dxxwjz2FolderExporter{}.export_db(context_of(dir, provider)));

  EXPECT_EQ(gunzip(read_binary(dir / "cfg" / "yolov4-obj.dxxwjz2")),
            expected_network(dir, {"cat"}));

  std::filesystem::remove_all(dir);
}

// the darknetxx trains no detector of no classes, but the export of an empty
// project still describes the network the YOLO v4 export does
TEST(UTEST_Dxxwjz2FolderExporter, describes_the_network_of_no_classes_too)
{
  const std::filesystem::path dir =
      std::filesystem::path{testing::TempDir()} / "utest_dxxwjz2_empty";
  std::filesystem::remove_all(dir);

  auto provider = std::make_shared<FakeProvider>();

  ASSERT_TRUE(Dxxwjz2FolderExporter{}.export_db(context_of(dir, provider)));

  const std::string document =
      gunzip(read_binary(dir / "cfg" / "yolov4-obj.dxxwjz2"));

  EXPECT_EQ(document, expected_network(dir, {}));
  EXPECT_NE(document.find("\"classes\": []}"), std::string::npos);

  std::filesystem::remove_all(dir);
}

TEST(UTEST_Dxxwjz2FolderExporter, writes_no_network_where_no_directory_is_made)
{
  const std::filesystem::path root =
      std::filesystem::path{testing::TempDir()} / "utest_dxxwjz2_blocked";
  std::filesystem::remove_all(root);
  std::filesystem::create_directories(root);

  // a regular file takes the place of the export directory
  const std::filesystem::path dir = root / "dst";
  write_file(dir, "a file, not a directory");

  auto provider = std::make_shared<FakeProvider>();
  provider->annotations = {"cat"};

  EXPECT_FALSE(Dxxwjz2FolderExporter{}.export_db(context_of(dir, provider)));
  EXPECT_TRUE(std::filesystem::is_regular_file(dir));

  std::filesystem::remove_all(root);
}
