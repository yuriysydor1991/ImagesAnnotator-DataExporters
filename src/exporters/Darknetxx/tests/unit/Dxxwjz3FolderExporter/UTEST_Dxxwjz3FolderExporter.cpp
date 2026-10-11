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

#include "src/exporters/Darknetxx/Dxxwjz3FolderExporter.h"
#include "src/exporters/Darknetxx/Dxxwjz3NetworkWriter.h"
#include "src/exporters/Yolo4/Yolo42FolderExporter.h"

using namespace testing;
using iannotator::exporters::Dxxwjz3FolderExporter;
using iannotator::exporters::Dxxwjz3NetworkWriter;
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

  EXPECT_TRUE((Dxxwjz3NetworkWriter{document, cfg, classes}.write()));

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

TEST(UTEST_Dxxwjz3FolderExporter, adds_the_json_network_to_the_yolo_layout)
{
  const std::filesystem::path dir =
      std::filesystem::path{testing::TempDir()} / "utest_dxxwjz3_export";
  std::filesystem::remove_all(dir);

  auto provider = std::make_shared<FakeProvider>();
  provider->annotations = {"cat", "dog"};

  ASSERT_TRUE(Dxxwjz3FolderExporter{}.export_db(context_of(dir, provider)));

  EXPECT_TRUE(std::filesystem::is_directory(dir / "backup"));
  EXPECT_EQ(read_file(dir / "data" / "obj.names"), "cat\ndog");
  ASSERT_TRUE(std::filesystem::is_regular_file(dir / "cfg" / "yolov4-obj.cfg"));

  // the dxxwjz3 network to train alone, of the cfg written into the file
  EXPECT_FALSE(std::filesystem::exists(dir / "cfg" / "yolov4-obj.dxxwjz2"));

  const std::filesystem::path network = dir / "cfg" / "yolov4-obj.dxxwjz3";

  ASSERT_TRUE(std::filesystem::is_regular_file(network));

  const std::string document = gunzip(read_binary(network));

  EXPECT_EQ(document, expected_network(dir, {"cat", "dog"}));
  EXPECT_EQ(
      document.rfind("{\n  \"format\": \"dxxwjz3\",\n  \"version\": 3,\n", 0),
      0U);
  EXPECT_NE(document.find("\"network\": {\"net\": {\"batch\": 64, "),
            std::string::npos);
  EXPECT_NE(document.find("\n  ], \"classes\": [\"cat\", \"dog\"]},\n"),
            std::string::npos);
}

// everything but the network to train is the YOLO v4 export byte for byte
TEST(UTEST_Dxxwjz3FolderExporter, writes_the_yolo_files_the_yolo4_export_does)
{
  const std::filesystem::path root =
      std::filesystem::path{testing::TempDir()} / "utest_dxxwjz3_parity";
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

  ASSERT_TRUE(
      Yolo42FolderExporter{}.export_db(context_of(root / "yolo", provider)));
  ASSERT_TRUE(Dxxwjz3FolderExporter{}.export_db(
      context_of(root / "darknetxx", provider)));

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

  EXPECT_EQ(compared, 7U);
  EXPECT_EQ(
      gunzip(read_binary(root / "darknetxx" / "cfg" / "yolov4-obj.dxxwjz3")),
      expected_network(root / "darknetxx", {"cat"}));

  std::filesystem::remove_all(root);
}

TEST(UTEST_Dxxwjz3FolderExporter, describes_the_network_of_no_classes_too)
{
  const std::filesystem::path dir =
      std::filesystem::path{testing::TempDir()} / "utest_dxxwjz3_empty";
  std::filesystem::remove_all(dir);

  auto provider = std::make_shared<FakeProvider>();

  ASSERT_TRUE(Dxxwjz3FolderExporter{}.export_db(context_of(dir, provider)));

  const std::string document =
      gunzip(read_binary(dir / "cfg" / "yolov4-obj.dxxwjz3"));

  EXPECT_EQ(document, expected_network(dir, {}));
  EXPECT_NE(document.find("\"classes\": []}"), std::string::npos);

  std::filesystem::remove_all(dir);
}
