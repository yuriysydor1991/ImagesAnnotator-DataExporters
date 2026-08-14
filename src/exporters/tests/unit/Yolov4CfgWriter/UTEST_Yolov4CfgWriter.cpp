#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "src/exporters/Yolov4CfgWriter.h"

using namespace testing;
using iannotator::exporters::Yolov4CfgWriter;

namespace
{

using Options = std::map<std::string, std::string>;

struct Section
{
  std::string name;
  Options options;
};

using Sections = std::vector<Section>;

/**
 * @brief Reads a descriptor the way the darknet parser does: every space of a
 * line dropped, the comment lines skipped and the rest a key=value of the
 * section the line stands under. Every section but the leading [net] one is a
 * network layer, so a section index is the darknet layer index plus one.
 */
Sections parse(const std::string& cfg)
{
  Sections sections;
  std::istringstream in{cfg};
  std::string line;

  while (std::getline(in, line)) {
    line.erase(
        std::remove_if(line.begin(), line.end(),
                       [](unsigned char c) { return std::isspace(c) != 0; }),
        line.end());

    if (line.empty() || line.front() == '#' || line.front() == ';') {
      continue;
    }

    if (line.front() == '[') {
      sections.emplace_back(Section{line, {}});
      continue;
    }

    const auto assign = line.find('=');

    if (assign == std::string::npos || sections.empty()) {
      continue;
    }

    sections.back().options[line.substr(0, assign)] = line.substr(assign + 1);
  }

  return sections;
}

std::string write(std::size_t classes, std::size_t images = 0)
{
  std::ostringstream out;

  Yolov4CfgWriter{out, classes, images}.write();

  return out.str();
}

std::size_t count_of(const Sections& sections, const std::string& name)
{
  return static_cast<std::size_t>(
      std::count_if(sections.begin(), sections.end(),
                    [&name](const Section& s) { return s.name == name; }));
}

}  // namespace

TEST(UTEST_Yolov4CfgWriter, writes_the_whole_yolov4_network)
{
  const Sections sections = parse(write(3));

  // the [net] section and the 162 layers of the YOLO v4 network
  ASSERT_EQ(sections.size(), 163U);
  EXPECT_EQ(sections.front().name, "[net]");

  EXPECT_EQ(count_of(sections, "[convolutional]"), 110U);
  EXPECT_EQ(count_of(sections, "[shortcut]"), 23U);
  EXPECT_EQ(count_of(sections, "[route]"), 21U);
  EXPECT_EQ(count_of(sections, "[maxpool]"), 3U);
  EXPECT_EQ(count_of(sections, "[upsample]"), 2U);
  EXPECT_EQ(count_of(sections, "[yolo]"), 3U);

  // darknet takes an input of a multiple of 32 only
  EXPECT_EQ(Yolov4CfgWriter::netWidth % 32U, 0U);
  EXPECT_EQ(Yolov4CfgWriter::netHeight % 32U, 0U);

  const Options& net = sections.front().options;

  EXPECT_EQ(net.at("width"), std::to_string(Yolov4CfgWriter::netWidth));
  EXPECT_EQ(net.at("height"), std::to_string(Yolov4CfgWriter::netHeight));
  EXPECT_EQ(net.at("channels"), "3");

  // the tail of the backbone, the layer the yolov4.conv.137 weights end at,
  // is the one darknet is asked to hold still over the first iterations
  EXPECT_EQ(sections.at(105).options.at("stopbackward"), "800");
}

TEST(UTEST_Yolov4CfgWriter, fills_the_class_count_into_every_detector)
{
  constexpr std::size_t classes{25};

  const Sections sections = parse(write(classes));
  const std::string filters = std::to_string((classes + 5) * 3);

  std::vector<std::string> masks;

  for (std::size_t i = 0; i < sections.size(); ++i) {
    if (sections[i].name != "[yolo]") {
      continue;
    }

    EXPECT_EQ(sections[i].options.at("classes"), std::to_string(classes));
    EXPECT_EQ(sections[i].options.at("num"), "9");

    masks.emplace_back(sections[i].options.at("mask"));

    // the convolution in front of a detector is the one carrying its boxes
    const Section& detector = sections.at(i - 1);

    EXPECT_EQ(detector.name, "[convolutional]");
    EXPECT_EQ(detector.options.at("filters"), filters);
    EXPECT_EQ(detector.options.at("activation"), "linear");
    EXPECT_EQ(detector.options.count("batch_normalize"), 0U);
  }

  EXPECT_EQ(masks, (std::vector<std::string>{"0,1,2", "3,4,5", "6,7,8"}));
}

TEST(UTEST_Yolov4CfgWriter, trains_2000_iterations_a_class_but_never_too_few)
{
  const Options few = parse(write(2, 10)).front().options;

  EXPECT_EQ(few.at("max_batches"), "6000");
  EXPECT_EQ(few.at("steps"), "4800,5400");

  const Options many = parse(write(25, 10)).front().options;

  EXPECT_EQ(many.at("max_batches"), "50000");
  EXPECT_EQ(many.at("steps"), "40000,45000");

  // and never fewer iterations than the dataset holds images
  const Options big = parse(write(3, 90000)).front().options;

  EXPECT_EQ(big.at("max_batches"), "90000");
  EXPECT_EQ(big.at("steps"), "72000,81000");
}

TEST(UTEST_Yolov4CfgWriter, routes_the_neck_into_the_backbone_feature_maps)
{
  const Sections sections = parse(write(3));

  std::vector<std::string> merged;

  for (const Section& section : sections) {
    if (section.name != "[route]" ||
        section.options.at("layers").front() == '-') {
      continue;
    }

    const std::size_t layer = std::stoul(section.options.at("layers"));

    ASSERT_LT(layer + 1, sections.size());
    EXPECT_EQ(sections[layer + 1].name, "[convolutional]");

    merged.emplace_back(sections[layer + 1].options.at("filters"));
  }

  // the stride 16 and the stride 8 outputs of the backbone, the maps the neck
  // upsamples its way down to
  EXPECT_EQ(merged, (std::vector<std::string>{"512", "256"}));
}
