#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

#include "src/exporters/Darknetxx/Dxxwjz2NetworkWriter.h"

using namespace testing;
using iannotator::exporters::Dxxwjz2NetworkWriter;

namespace
{

/// @brief The U+FFFD replacement character in UTF-8
const std::string replacement{"\xEF\xBF\xBD"};

std::string document_of(const std::string& cfg,
                        const std::vector<std::string>& classes)
{
  std::ostringstream document;

  Dxxwjz2NetworkWriter{document, cfg, classes}.write();

  return document.str();
}

}  // namespace

// the very text the darknetxx converter writes of a cfg file and a class
// names file, but the generator
TEST(UTEST_Dxxwjz2NetworkWriter, writes_the_document_of_the_network_to_train)
{
  const std::string expected =
      "{\n"
      "  \"format\": \"dxxwjz2\",\n"
      "  \"version\": 2,\n"
      "  \"generator\": \"ImagesAnnotatorDataExporters-0.14 0.14.0\",\n"
      "  \"network\": {\"cfg\": \"[net]\\nbatch=64\\n\", \"classes\": [\n"
      "    {\"index\": 0, \"name\": \"cat\"},\n"
      "    {\"index\": 1, \"name\": \"dog\"}\n"
      "  ]},\n"
      "  \"training\": {\"seconds\": 0,\n"
      "    \"iteration\": [],\n"
      "    \"loss\": [],\n"
      "    \"avg_loss\": []\n"
      "  }\n"
      "}\n";

  EXPECT_EQ(document_of("[net]\nbatch=64\n", {"cat", "dog"}), expected);
}

TEST(UTEST_Dxxwjz2NetworkWriter, closes_an_empty_classes_array_on_its_line)
{
  const std::string document = document_of("[net]\n", {});

  EXPECT_NE(document.find("  \"network\": {\"cfg\": \"[net]\\n\", "
                          "\"classes\": []},\n  \"training\""),
            std::string::npos);
}

TEST(UTEST_Dxxwjz2NetworkWriter, numbers_the_classes_in_their_given_order)
{
  const std::string document = document_of("[net]\n", {"b", "a", "c"});

  EXPECT_NE(document.find("    {\"index\": 0, \"name\": \"b\"},\n"
                          "    {\"index\": 1, \"name\": \"a\"},\n"
                          "    {\"index\": 2, \"name\": \"c\"}\n  ]}"),
            std::string::npos);
}

TEST(UTEST_Dxxwjz2NetworkWriter, writes_the_generator_of_this_library)
{
  EXPECT_EQ(Dxxwjz2NetworkWriter::generator(),
            "ImagesAnnotatorDataExporters-0.14 0.14.0");
}

TEST(UTEST_Dxxwjz2NetworkWriter, quotes_the_names_written_into_the_document)
{
  const std::string document = document_of("[net]\n", {"say \"hi\""});

  EXPECT_NE(document.find("{\"index\": 0, \"name\": \"say \\\"hi\\\"\"}"),
            std::string::npos);
}

TEST(UTEST_Dxxwjz2NetworkWriter, quote_escapes_what_json_does_not_take_as_is)
{
  EXPECT_EQ(Dxxwjz2NetworkWriter::quote("a\"b\\c"), "\"a\\\"b\\\\c\"");
  EXPECT_EQ(Dxxwjz2NetworkWriter::quote("l1\nl2\r\tx"), "\"l1\\nl2\\r\\tx\"");
  EXPECT_EQ(Dxxwjz2NetworkWriter::quote(std::string{"\x00\x01\x1F", 3}),
            "\"\\u0000\\u0001\\u001f\"");
}

// what JSON takes as it is stays the way it is, the slash and the DEL
// included, exactly as the darknetxx writes them
TEST(UTEST_Dxxwjz2NetworkWriter, quote_keeps_the_rest_of_the_ascii)
{
  EXPECT_EQ(Dxxwjz2NetworkWriter::quote(""), "\"\"");
  EXPECT_EQ(Dxxwjz2NetworkWriter::quote("cfg/yolov4-obj.cfg\x7F"),
            "\"cfg/yolov4-obj.cfg\x7F\"");
}

TEST(UTEST_Dxxwjz2NetworkWriter, quote_keeps_the_valid_utf8_sequences)
{
  // the two, three and four bytes long sequences
  const std::string text{
      "\xD0\xBA\xD1\x96\xD1\x82 \xE2\x82\xAC \xF0\x9F\x98\x80"};

  EXPECT_EQ(Dxxwjz2NetworkWriter::quote(text), "\"" + text + "\"");
}

TEST(UTEST_Dxxwjz2NetworkWriter, quote_replaces_every_byte_which_isnt_utf8)
{
  // a Latin-1 letter
  EXPECT_EQ(Dxxwjz2NetworkWriter::quote("caf\xE9"),
            "\"caf" + replacement + "\"");
  // the overlong slash
  EXPECT_EQ(Dxxwjz2NetworkWriter::quote("\xC0\xAF"),
            "\"" + replacement + replacement + "\"");
  // a surrogate half
  EXPECT_EQ(Dxxwjz2NetworkWriter::quote("\xED\xA0\x80"),
            "\"" + replacement + replacement + replacement + "\"");
  // beyond U+10FFFF
  EXPECT_EQ(
      Dxxwjz2NetworkWriter::quote("\xF4\x90\x80\x80"),
      "\"" + replacement + replacement + replacement + replacement + "\"");
  // a sequence the text ends in the middle of
  EXPECT_EQ(Dxxwjz2NetworkWriter::quote("a\xE2\x82"),
            "\"a" + replacement + replacement + "\"");
  // the bytes no sequence starts with
  EXPECT_EQ(Dxxwjz2NetworkWriter::quote("\x80\xFF"),
            "\"" + replacement + replacement + "\"");
}

// the darknetxx refuses the network description of no cfg text
TEST(UTEST_Dxxwjz2NetworkWriter, writes_nothing_of_no_cfg_text)
{
  std::ostringstream document;

  EXPECT_FALSE((Dxxwjz2NetworkWriter{document, "", {"cat"}}.write()));
  EXPECT_TRUE(document.str().empty());
}
