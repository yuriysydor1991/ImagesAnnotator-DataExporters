#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <string>
#include <vector>

#include "src/exporters/Darknetxx/Dxxwjz3CfgJson.h"

using namespace testing;
using iannotator::exporters::Dxxwjz3CfgJson;

namespace
{

std::string value_of(const std::string& text)
{
  return Dxxwjz3CfgJson::json_of(std::optional<std::string>{text});
}

}  // namespace

TEST(UTEST_Dxxwjz3CfgJson, keeps_the_net_options_and_the_layer_sections)
{
  const Dxxwjz3CfgJson json{
      "[net]\nbatch=64\nhue=.1\n\n"
      "[convolutional]\nfilters=32\nactivation=mish\n\n"
      "[route]\nlayers=-1, -7\n\n"
      "[maxpool]\n"};

  ASSERT_TRUE(json.valid());
  EXPECT_EQ(json.net(), "\"batch\": 64, \"hue\": 0.1");
  EXPECT_THAT(
      json.layers(),
      ElementsAre("\"type\": \"convolutional\", \"options\": {\"filters\": 32, "
                  "\"activation\": \"mish\"}",
                  "\"type\": \"route\", \"options\": {\"layers\": [-1,-7]}",
                  "\"type\": \"maxpool\""));
}

// the darknet drops every whitespace of a line and skips the comments
TEST(UTEST_Dxxwjz3CfgJson, reads_the_cfg_text_the_way_the_darknet_does)
{
  const Dxxwjz3CfgJson json{
      "# the network\n[net]\n; a comment\n  batch = 64 \r\n\t\n"
      "width=416 # not a comment\n[ yolo ]\n"};

  ASSERT_TRUE(json.valid());
  EXPECT_EQ(json.net(), "\"batch\": 64, \"width\": \"416#notacomment\"");
  EXPECT_THAT(json.layers(), ElementsAre("\"type\": \"yolo\""));
}

// the darknet reads the first option of a key, the line of no '=' is the key of
// the NULL value and the line of nothing after the '=' is dropped
TEST(UTEST_Dxxwjz3CfgJson, keeps_the_options_the_darknet_reads)
{
  const Dxxwjz3CfgJson json{"[network]\nbatch=64\nbatch=1\nempty=\nrandom\n"};

  ASSERT_TRUE(json.valid());
  EXPECT_EQ(json.net(), "\"batch\": 64, \"random\": null");
  EXPECT_TRUE(json.layers().empty());
}

TEST(UTEST_Dxxwjz3CfgJson, refuses_the_cfg_the_darknetxx_refuses)
{
  // no section at all
  EXPECT_FALSE(Dxxwjz3CfgJson{""}.valid());
  EXPECT_FALSE(Dxxwjz3CfgJson{"# a comment alone\n"}.valid());
  // an option before the first section
  EXPECT_FALSE(Dxxwjz3CfgJson{"batch=64\n[net]\n"}.valid());
  // the first section isn't the [net] one
  EXPECT_FALSE(Dxxwjz3CfgJson{"[convolutional]\n[net]\n"}.valid());
  // a section header of no closing bracket or of no name
  EXPECT_FALSE(Dxxwjz3CfgJson{"[net]\n[convolutional\n"}.valid());
  EXPECT_FALSE(Dxxwjz3CfgJson{"[net]\n[]\n"}.valid());
  EXPECT_TRUE(Dxxwjz3CfgJson{"[net]\n"}.valid());
}

TEST(UTEST_Dxxwjz3CfgJson, writes_the_integers_as_the_integers)
{
  EXPECT_EQ(value_of("64"), "64");
  EXPECT_EQ(value_of("-1"), "-1");
  EXPECT_EQ(value_of("+5"), "5");
  EXPECT_EQ(value_of("-0"), "0");
  EXPECT_EQ(value_of("9223372036854775807"), "9223372036854775807");
}

TEST(UTEST_Dxxwjz3CfgJson, writes_the_fractions_of_their_shortest_decimals)
{
  EXPECT_EQ(value_of(".1"), "0.1");
  EXPECT_EQ(value_of("0.0005"), "0.0005");
  EXPECT_EQ(value_of("1.50"), "1.5");
  EXPECT_EQ(value_of("1.0"), "1");
  EXPECT_EQ(value_of("0.949"), "0.949");
  EXPECT_EQ(value_of("-2.5"), "-2.5");
  EXPECT_EQ(value_of("1."), "1");
}

// the number of the atoi() other than the one of the text stays the text
TEST(UTEST_Dxxwjz3CfgJson, writes_the_rest_as_the_strings)
{
  EXPECT_EQ(value_of("1e-3"), "\"1e-3\"");
  EXPECT_EQ(value_of("1e2"), "\"1e2\"");
  EXPECT_EQ(value_of("0x10"), "\"0x10\"");
  EXPECT_EQ(value_of("nan"), "\"nan\"");
  EXPECT_EQ(value_of("leaky"), "\"leaky\"");
  EXPECT_EQ(value_of("data/train.txt"), "\"data/train.txt\"");
  EXPECT_EQ(value_of("-"), "\"-\"");
  EXPECT_EQ(value_of("say\"hi\""), "\"say\\\"hi\\\"\"");
}

TEST(UTEST_Dxxwjz3CfgJson, writes_the_comma_separated_values_as_the_arrays)
{
  EXPECT_EQ(value_of("0,1,2"), "[0,1,2]");
  EXPECT_EQ(value_of(".1,.1"), "[0.1,0.1]");
  EXPECT_EQ(value_of("4800,mish,.5"), "[4800,\"mish\",0.5]");
  EXPECT_EQ(Dxxwjz3CfgJson::json_of(std::nullopt), "null");
}

TEST(UTEST_Dxxwjz3CfgJson, fraction_text_is_the_shortest_fixed_decimal)
{
  EXPECT_EQ(Dxxwjz3CfgJson::fraction_text(0.1f), "0.1");
  EXPECT_EQ(Dxxwjz3CfgJson::fraction_text(1.f), "1");
  EXPECT_EQ(Dxxwjz3CfgJson::fraction_text(0.f), "0");
  EXPECT_EQ(Dxxwjz3CfgJson::fraction_text(1.05f), "1.05");
  EXPECT_EQ(Dxxwjz3CfgJson::fraction_text(-0.213f), "-0.213");
  // the least denormal float
  EXPECT_EQ(Dxxwjz3CfgJson::fraction_text(1e-45f),
            "0.000000000000000000000000000000000000000000001");
}
