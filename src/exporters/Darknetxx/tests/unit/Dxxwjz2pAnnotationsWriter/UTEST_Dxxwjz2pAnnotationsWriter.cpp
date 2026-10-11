#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <string>

#include "src/exporters/Darknetxx/Dxxwjz2pAnnotationsWriter.h"

using namespace testing;
using iannotator::exporters::Dxxwjz2pAnnotationsWriter;
using iannotator::exporters::ImageRecord;
using iannotator::exporters::ImageRecordPtr;
using iannotator::exporters::ImageRecordRect;

namespace
{

ImageRecordPtr record_of(int width, int height)
{
  auto ir = ImageRecord::create("ignored.png", "/ignored");
  ir->iwidth = width;
  ir->iheight = height;
  return ir;
}

std::string member_of(const std::string& folder,
                      const Dxxwjz2pAnnotationsWriter::Images& images)
{
  std::ostringstream member;

  Dxxwjz2pAnnotationsWriter{member, folder, images}.write();

  return member.str();
}

}  // namespace

// the very text the darknetxx writes of the images of a folder: an image on a
// line, its boxes on its line, the rects of the pixels of the image
TEST(UTEST_Dxxwjz2pAnnotationsWriter, writes_the_images_of_the_folder)
{
  auto park = record_of(640, 480);
  park->rects.emplace_back(
      std::make_shared<ImageRecordRect>("dog", 200, 130, 48, 52));
  park->rects.emplace_back(
      std::make_shared<ImageRecordRect>("cat", 0, 0, 640, 480));

  auto street = record_of(320, 200);
  street->rects.emplace_back(
      std::make_shared<ImageRecordRect>("cat", 1, 2, 3, 4));

  const std::string expected =
      ",\n"
      "  \"annotations\": {\"ImagesAnnotator\": {\"version\": \"0.12.0\"}, "
      "\"annotations\": [\n"
      "    {\"db\": {\"path\": \"images\"}, \"annotations\": [\n"
      "      {\"rel_path\": \"park.jpg\", \"width\": 640, \"height\": 480, "
      "\"annotations\": [{\"name\": \"dog\", \"rect\": {\"x\": 200, "
      "\"y\": 130, \"width\": 48, \"height\": 52}}, {\"name\": \"cat\", "
      "\"rect\": {\"x\": 0, \"y\": 0, \"width\": 640, \"height\": 480}}]},\n"
      "      {\"rel_path\": \"street-1.png\", \"width\": 320, \"height\": 200, "
      "\"annotations\": [{\"name\": \"cat\", \"rect\": {\"x\": 1, \"y\": 2, "
      "\"width\": 3, \"height\": 4}}]}\n"
      "    ]}\n"
      "  ]}";

  EXPECT_EQ(member_of("images", {{"park.jpg", park}, {"street-1.png", street}}),
            expected);
}

TEST(UTEST_Dxxwjz2pAnnotationsWriter, writes_the_image_of_no_box)
{
  const std::string member = member_of("images", {{"a.png", record_of(2, 3)}});

  EXPECT_NE(member.find("\n      {\"rel_path\": \"a.png\", \"width\": 2, "
                        "\"height\": 3, \"annotations\": []}\n    ]}"),
            std::string::npos);
}

// the darknetxx groups the images by their folders, of none for no image
TEST(UTEST_Dxxwjz2pAnnotationsWriter, writes_no_folder_of_no_image)
{
  EXPECT_EQ(member_of("images", {}),
            ",\n  \"annotations\": {\"ImagesAnnotator\": {\"version\": "
            "\"0.12.0\"}, \"annotations\": []}");
}

TEST(UTEST_Dxxwjz2pAnnotationsWriter, quotes_the_paths_and_the_names)
{
  auto ir = record_of(10, 10);
  ir->rects.emplace_back(
      std::make_shared<ImageRecordRect>("say \"hi\"", 1, 1, 2, 2));

  const std::string member = member_of("my \"images\"", {{"a\\b.png", ir}});

  EXPECT_NE(member.find("{\"db\": {\"path\": \"my \\\"images\\\"\"}"),
            std::string::npos);
  EXPECT_NE(member.find("{\"rel_path\": \"a\\\\b.png\""), std::string::npos);
  EXPECT_NE(member.find("{\"name\": \"say \\\"hi\\\"\""), std::string::npos);
}
