#include <ImagesAnnotatorDataDrivers-0.13/IADataDriversFacade.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <zlib.h>

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "IADataExportersFacade.h"
#include "IImageCropperFacility.h"

namespace iadd = ImagesAnnotatorDataDrivers013;
namespace iade = ImagesAnnotatorDataExporters013;

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

std::string read_binary(const std::filesystem::path& p)
{
  std::ifstream f(p, std::ios::binary);

  return {std::istreambuf_iterator<char>{f}, std::istreambuf_iterator<char>{}};
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

    auto factory = iadd::IADataDriversFacade::create_records_factory();

    ASSERT_NE(factory, nullptr);

    auto ir = factory->create_image_record("a.png", imagesDir.string());

    ASSERT_NE(ir, nullptr);

    ir->iwidth = 200;
    ir->iheight = 100;
    ir->rects.emplace_back(
        factory->create_image_rect_record("dog", 50, 20, 100, 40));

    db = iadd::IADataDriversFacade::create_annotations_db();

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

  auto ctx = context<iade::PlainTxtExportContext>("plain");

  auto exporter = iade::IADataExportersFacade::create_exporter(ctx);

  ASSERT_NE(exporter, nullptr);
  ASSERT_TRUE(exporter->export_db(ctx));

  ASSERT_TRUE(std::filesystem::is_regular_file(dir / "dog.txt"));
  EXPECT_EQ(read_file(dir / "dog.txt"),
            (imagesDir / "a.png").string() + " 1 50 20 100 40");
}

TEST_F(CTEST_Exporters, yolo4_export_writes_the_darknet_layout)
{
  auto ctx = context<iade::Yolo4ExportContext>("yolo");

  auto exporter = iade::IADataExportersFacade::create_exporter(ctx);

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
  auto ctx = context<iade::UltralyticsDetectExportContext>("ul-detect");

  auto exporter = iade::IADataExportersFacade::create_exporter(ctx);

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
  auto ctx = context<iade::UltralyticsObbExportContext>("ul-obb");

  auto exporter = iade::IADataExportersFacade::create_exporter(ctx);

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
  auto ctx = context<iade::UltralyticsSegmentExportContext>("ul-seg");

  auto exporter = iade::IADataExportersFacade::create_exporter(ctx);

  ASSERT_NE(exporter, nullptr);
  ASSERT_TRUE(exporter->export_db(ctx));

  EXPECT_EQ(read_file(root / "ul-seg" / "labels" / "train" / "a.txt"),
            ultralyticsCorners);
}

TEST_F(CTEST_Exporters, coco_export_writes_the_pixel_bbox_of_the_rectangle)
{
  auto ctx = context<iade::CocoExportContext>("coco");

  auto exporter = iade::IADataExportersFacade::create_exporter(ctx);

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
  auto ctx = context<iade::PascalVocExportContext>("voc");

  auto exporter = iade::IADataExportersFacade::create_exporter(ctx);

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
  auto ctx = context<iade::CreateMLExportContext>("createml");

  auto exporter = iade::IADataExportersFacade::create_exporter(ctx);

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

// The YOLO v4 directory once more, which the darknetxx-gui trains, with the
// gzip compressed JSON document of the network to train beside its cfg file:
// the cfg text and the class names the labels number
TEST_F(CTEST_Exporters, dxxwjz2_export_adds_the_network_to_train)
{
  auto ctx = iade::IADataExportersFacade::create_dxxwjz2_library_context();

  ASSERT_NE(ctx, nullptr);

  ctx->set_export_path((root / "dxxwjz2").string());
  ctx->set_db_provider(db);

  auto exporter = iade::IADataExportersFacade::create_exporter(ctx);

  ASSERT_NE(exporter, nullptr);
  ASSERT_TRUE(exporter->export_db(ctx));

  const std::filesystem::path dir = root / "dxxwjz2";

  EXPECT_TRUE(std::filesystem::is_directory(dir / "backup"));
  EXPECT_EQ(read_file(dir / "data" / "obj.names"), "dog");
  EXPECT_EQ(read_file(dir / "data" / "a.txt"), "0 0.5 0.4 0.5 0.4");

  const std::string cfg = read_binary(dir / "cfg" / "yolov4-obj.cfg");
  const std::string member = read_binary(dir / "cfg" / "yolov4-obj.dxxwjz2");

  ASSERT_FALSE(cfg.empty());
  ASSERT_GE(member.size(), 2U);
  EXPECT_EQ(member.substr(0, 2), "\x1F\x8B");

  const std::string document = gunzip(member);

  EXPECT_EQ(document.rfind("{\n  \"format\": \"dxxwjz2\",\n"
                           "  \"version\": 2,\n",
                           0),
            0U);
  // the first line of the cfg text, as the JSON string keeps it
  EXPECT_NE(document.find("\"network\": {\"cfg\": \"" +
                          cfg.substr(0, cfg.find('\n')) + "\\n"),
            std::string::npos);
  EXPECT_NE(document.find("\"classes\": [\n"
                          "    {\"index\": 0, \"name\": \"dog\"}\n  ]}"),
            std::string::npos);
  EXPECT_NE(document.find("\"training\": {\"seconds\": 0,"), std::string::npos);
}

// The YOLO v4 directory of the darknetxx once more, of the dxxwjz3 network to
// train: the cfg options of the JSON values instead of the cfg text
TEST_F(CTEST_Exporters, dxxwjz3_export_adds_the_json_network_to_train)
{
  auto ctx = iade::IADataExportersFacade::create_dxxwjz3_library_context();

  ASSERT_NE(ctx, nullptr);

  ctx->set_export_path((root / "dxxwjz3").string());
  ctx->set_db_provider(db);

  auto exporter = iade::IADataExportersFacade::create_exporter(ctx);

  ASSERT_NE(exporter, nullptr);
  ASSERT_TRUE(exporter->export_db(ctx));

  const std::filesystem::path dir = root / "dxxwjz3";

  EXPECT_TRUE(std::filesystem::is_directory(dir / "backup"));
  EXPECT_EQ(read_file(dir / "data" / "a.txt"), "0 0.5 0.4 0.5 0.4");
  EXPECT_TRUE(std::filesystem::is_regular_file(dir / "cfg" / "yolov4-obj.cfg"));
  EXPECT_FALSE(std::filesystem::exists(dir / "cfg" / "yolov4-obj.dxxwjz2"));

  const std::string document =
      gunzip(read_binary(dir / "cfg" / "yolov4-obj.dxxwjz3"));

  EXPECT_EQ(document.rfind("{\n  \"format\": \"dxxwjz3\",\n"
                           "  \"version\": 3,\n",
                           0),
            0U);
  EXPECT_NE(document.find("\"network\": {\"net\": {\"batch\": 64, "),
            std::string::npos);
  EXPECT_NE(document.find("\n  ], \"classes\": [\"dog\"]},\n"),
            std::string::npos);
  EXPECT_EQ(document.find("\"cfg\""), std::string::npos);
}

// The darknetxx training projects: the one file of the network, the class
// names and the annotations beside the copies of the images
TEST_F(CTEST_Exporters, darknetxx_project_exports_write_the_project_files)
{
  const std::vector<std::pair<iade::IADataExportersContextPtr, std::string>>
      projects{
          {iade::IADataExportersFacade::create_dxxwjz2p_library_context(),
           "dxxwjz2p"},
          {iade::IADataExportersFacade::create_dxxwjz3p_library_context(),
           "dxxwjz3p"},
      };

  for (const auto& [ctx, format] : projects) {
    ASSERT_NE(ctx, nullptr) << format;

    const std::filesystem::path dir = root / format;

    ctx->set_export_path(dir.string());
    ctx->set_db_provider(db);

    auto exporter = iade::IADataExportersFacade::create_exporter(ctx);

    ASSERT_NE(exporter, nullptr) << format;
    ASSERT_TRUE(exporter->export_db(ctx)) << format;

    EXPECT_TRUE(std::filesystem::is_directory(dir / "backup")) << format;
    EXPECT_EQ(read_file(dir / "images" / "a.png"), "not-a-real-image")
        << format;
    // none of the YOLO v4 directory
    EXPECT_FALSE(std::filesystem::exists(dir / "data")) << format;
    EXPECT_FALSE(std::filesystem::exists(dir / "cfg")) << format;

    const std::string document =
        gunzip(read_binary(dir / ("yolov4-obj." + format)));

    EXPECT_EQ(
        document.rfind(
            "{\n  \"format\": \"" + format + "\",\n  \"version\": 1,\n", 0),
        0U)
        << format;
    EXPECT_NE(
        document.find(
            ",\n  \"annotations\": {\"ImagesAnnotator\": {\"version\": "
            "\"0.12.0\"}, \"annotations\": [\n"
            "    {\"db\": {\"path\": \"images\"}, \"annotations\": [\n"
            "      {\"rel_path\": \"a.png\", \"width\": 200, \"height\": "
            "100, \"annotations\": [{\"name\": \"dog\", \"rect\": {\"x\": "
            "50, \"y\": 20, \"width\": 100, \"height\": 40}}]}\n"
            "    ]}\n  ]}\n}\n"),
        std::string::npos)
        << format;
  }
}

TEST_F(CTEST_Exporters, pytorch_vision_export_crops_into_the_tag_directory)
{
  auto ctx = context<iade::PyTorchExportContext>("pytorch");
  ctx->set_cropper(std::make_shared<FakeCropper>());

  std::filesystem::create_directories(ctx->get_export_path());

  auto exporter = iade::IADataExportersFacade::create_exporter(ctx);

  ASSERT_NE(exporter, nullptr);
  ASSERT_TRUE(exporter->export_db(ctx));

  const std::filesystem::path dir = root / "pytorch";

  ASSERT_TRUE(std::filesystem::is_directory(dir / "dog"));
  EXPECT_EQ(read_file(dir / "dog" / "a.png"), "cropped");
}

TEST_F(CTEST_Exporters,
       perform_export_runs_the_export_named_by_the_library_context)
{
  auto ctx = std::make_shared<iade::Yolo4ExportContext>();

  ctx->set_export_path((root / "perform_export").string());
  ctx->set_db_provider(db);

  auto lib = iade::IADataExportersFacade::create_library(ctx);

  ASSERT_NE(lib, nullptr);
  ASSERT_TRUE(lib->perform_export(ctx));

  EXPECT_NE(ctx->get_exporter(), nullptr);
  EXPECT_EQ(read_file(root / "perform_export" / "data" / "obj.names"), "dog");
}

TEST_F(CTEST_Exporters, library_version_matches_the_data_drivers_one_it_reads)
{
  EXPECT_FALSE(iade::IADataExportersFacade::library_version().empty());
  EXPECT_FALSE(iadd::IADataDriversFacade::library_version().empty());
}
