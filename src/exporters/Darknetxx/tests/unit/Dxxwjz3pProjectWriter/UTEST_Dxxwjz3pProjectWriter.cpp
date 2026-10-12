#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <string>

#include "src/exporters/Darknetxx/Dxxwjz3pProjectWriter.h"

using namespace testing;
using iannotator::exporters::Dxxwjz3pProjectWriter;
using iannotator::exporters::ImageRecord;
using iannotator::exporters::ImageRecordRect;

// the dxxwjz3 document of the network to train, of the dxxwjz3p format and
// version, with the dxxwjz2p annotations member after the training record one
TEST(UTEST_Dxxwjz3pProjectWriter, writes_the_document_of_the_project_to_train)
{
  auto ir = ImageRecord::create("a.png", "/src");
  ir->iwidth = 200;
  ir->iheight = 100;
  ir->rects.emplace_back(
      std::make_shared<ImageRecordRect>("dog", 50, 20, 100, 40));

  const std::string expected =
      "{\n"
      "  \"format\": \"dxxwjz3p\",\n"
      "  \"version\": 1,\n"
      "  \"generator\": \"ImagesAnnotatorDataExporters-0.14 0.14.0\",\n"
      "  \"network\": {\"net\": {\"batch\": 64}, \"layers\": [\n"
      "    {\"type\": \"maxpool\", \"options\": {\"size\": 2}}\n"
      "  ], \"classes\": [\"cat\", \"dog\"]},\n"
      "  \"training\": {\"seconds\": 0,\n"
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

  EXPECT_TRUE((Dxxwjz3pProjectWriter{document,
                                     "[net]\nbatch=64\n[maxpool]\nsize=2\n",
                                     {"cat", "dog"},
                                     "images",
                                     {{"a.png", ir}}}
                   .write()));
  EXPECT_EQ(document.str(), expected);
}

TEST(UTEST_Dxxwjz3pProjectWriter, writes_nothing_of_the_cfg_it_cannot_keep)
{
  std::ostringstream document;

  EXPECT_FALSE((Dxxwjz3pProjectWriter{
      document, "[maxpool]\nsize=2\n", {"cat"}, "images", {}}
                    .write()));
  EXPECT_TRUE(document.str().empty());
}

TEST(UTEST_Dxxwjz3pProjectWriter, names_the_project_file_extension)
{
  EXPECT_EQ(Dxxwjz3pProjectWriter::extension, ".dxxwjz3p");
}
