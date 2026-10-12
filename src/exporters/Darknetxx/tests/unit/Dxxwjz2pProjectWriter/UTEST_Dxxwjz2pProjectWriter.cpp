#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <string>

#include "src/exporters/Darknetxx/Dxxwjz2pProjectWriter.h"

using namespace testing;
using iannotator::exporters::Dxxwjz2pProjectWriter;
using iannotator::exporters::ImageRecord;
using iannotator::exporters::ImageRecordRect;

// the dxxwjz2 document of the network to train, of the dxxwjz2p format and
// version, with the annotations member after the training record one
TEST(UTEST_Dxxwjz2pProjectWriter, writes_the_document_of_the_project_to_train)
{
  auto ir = ImageRecord::create("a.png", "/src");
  ir->iwidth = 200;
  ir->iheight = 100;
  ir->rects.emplace_back(
      std::make_shared<ImageRecordRect>("dog", 50, 20, 100, 40));

  const std::string expected =
      "{\n"
      "  \"format\": \"dxxwjz2p\",\n"
      "  \"version\": 1,\n"
      "  \"generator\": \"ImagesAnnotatorDataExporters-0.13 0.13.0\",\n"
      "  \"network\": {\"cfg\": \"[net]\\nbatch=64\\n\", \"classes\": [\n"
      "    {\"index\": 0, \"name\": \"cat\"},\n"
      "    {\"index\": 1, \"name\": \"dog\"}\n"
      "  ]},\n"
      "  \"training\": {\"seconds\": 0,\n"
      "    \"iteration\": [],\n"
      "    \"loss\": [],\n"
      "    \"avg_loss\": []\n"
      "  },\n"
      "  \"annotations\": {\"ImagesAnnotator\": {\"version\": \"0.12.0\"}, "
      "\"annotations\": [\n"
      "    {\"db\": {\"path\": \"images\"}, \"annotations\": [\n"
      "      {\"rel_path\": \"a.png\", \"width\": 200, \"height\": 100, "
      "\"annotations\": [{\"name\": \"dog\", \"rect\": {\"x\": 50, \"y\": 20, "
      "\"width\": 100, \"height\": 40}}]}\n"
      "    ]}\n"
      "  ]}\n"
      "}\n";

  std::ostringstream document;

  EXPECT_TRUE((Dxxwjz2pProjectWriter{
      document, "[net]\nbatch=64\n", {"cat", "dog"}, "images", {{"a.png", ir}}}
                   .write()));
  EXPECT_EQ(document.str(), expected);
}

TEST(UTEST_Dxxwjz2pProjectWriter, writes_nothing_of_no_cfg_text)
{
  std::ostringstream document;

  EXPECT_FALSE(
      (Dxxwjz2pProjectWriter{document, "", {"cat"}, "images", {}}.write()));
  EXPECT_TRUE(document.str().empty());
}

TEST(UTEST_Dxxwjz2pProjectWriter, names_the_project_file_extension)
{
  EXPECT_EQ(Dxxwjz2pProjectWriter::extension, ".dxxwjz2p");
}
