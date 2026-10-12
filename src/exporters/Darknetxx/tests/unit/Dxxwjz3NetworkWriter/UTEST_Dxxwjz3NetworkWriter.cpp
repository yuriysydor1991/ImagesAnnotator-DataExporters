#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

#include "src/exporters/Darknetxx/Dxxwjz3NetworkWriter.h"

using namespace testing;
using iannotator::exporters::Dxxwjz3NetworkWriter;

namespace
{

std::string document_of(const std::string& cfg,
                        const std::vector<std::string>& classes)
{
  std::ostringstream document;

  EXPECT_TRUE((Dxxwjz3NetworkWriter{document, cfg, classes}.write()));

  return document.str();
}

}  // namespace

// the very text the darknetxx converter writes of a cfg file and a class
// names file, but the generator
TEST(UTEST_Dxxwjz3NetworkWriter, writes_the_document_of_the_network_to_train)
{
  const std::string expected =
      "{\n"
      "  \"format\": \"dxxwjz3\",\n"
      "  \"version\": 3,\n"
      "  \"generator\": \"ImagesAnnotatorDataExporters-0.13 0.13.0\",\n"
      "  \"network\": {\"net\": {\"batch\": 64, \"hue\": 0.1}, \"layers\": [\n"
      "    {\"type\": \"convolutional\", \"options\": {\"filters\": 32, "
      "\"activation\": \"mish\"}},\n"
      "    {\"type\": \"yolo\", \"options\": {\"mask\": [0,1,2], "
      "\"classes\": 2}}\n"
      "  ], \"classes\": [\"cat\", \"dog\"]},\n"
      "  \"training\": {\"seconds\": 0,\n"
      "    \"loss\": [],\n"
      "    \"avg_loss\": []\n"
      "  }\n"
      "}\n";

  EXPECT_EQ(document_of("# the comments go\n[net]\nbatch=64\nhue=.1\n\n"
                        "[convolutional]\nfilters=32\nactivation=mish\n\n"
                        "[yolo]\nmask=0,1,2\nclasses=2\n",
                        {"cat", "dog"}),
            expected);
}

TEST(UTEST_Dxxwjz3NetworkWriter, closes_the_empty_arrays_on_their_line)
{
  const std::string document = document_of("[net]\nbatch=1\n", {});

  EXPECT_NE(document.find("  \"network\": {\"net\": {\"batch\": 1}, "
                          "\"layers\": [], \"classes\": []},\n  \"training\""),
            std::string::npos);
}

TEST(UTEST_Dxxwjz3NetworkWriter, quotes_the_class_names_in_their_given_order)
{
  const std::string document =
      document_of("[net]\n", {"b", "say \"hi\"", "\xD0\xBA\xD1\x96\xD1\x82"});

  EXPECT_NE(document.find("\"classes\": [\"b\", \"say \\\"hi\\\"\", "
                          "\"\xD0\xBA\xD1\x96\xD1\x82\"]}"),
            std::string::npos);
}

TEST(UTEST_Dxxwjz3NetworkWriter, writes_nothing_of_the_cfg_it_cannot_keep)
{
  for (const std::string cfg : {"", "batch=64\n[net]\n", "[yolo]\n"}) {
    std::ostringstream document;

    EXPECT_FALSE((Dxxwjz3NetworkWriter{document, cfg, {"cat"}}.write())) << cfg;
    EXPECT_TRUE(document.str().empty()) << cfg;
  }
}
