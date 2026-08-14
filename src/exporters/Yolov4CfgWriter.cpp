/**
 * The "Simplified BSD License"
 *
 * Copyright (c) 2025, Yurii Sydor (yuriysydor1991@gmail.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "src/exporters/Yolov4CfgWriter.h"

#include <algorithm>
#include <cstddef>
#include <ostream>
#include <string>
#include <utility>

namespace iannotator::exporters
{

namespace
{
/// @brief The activation of the backbone, of the neck and of the detectors.
const std::string mish{"mish"};
const std::string leaky{"leaky"};
const std::string linear{"linear"};
}  // namespace

Yolov4CfgWriter::Yolov4CfgWriter(std::ostream& destination,
                                 std::size_t classesCount,
                                 std::size_t imagesCount)
    : stream{destination}, classes{classesCount}, images{imagesCount}
{
}

void Yolov4CfgWriter::write()
{
  net_section();

  neck(backbone());

  heads();
}

std::size_t Yolov4CfgWriter::iterations() const
{
  return std::max({minIterations, classes * iterationsPerClass, images});
}

std::size_t Yolov4CfgWriter::yolo_filters() const
{
  constexpr std::size_t boxAndObjectness{5};
  constexpr std::size_t anchorsPerHead{3};

  return (classes + boxAndObjectness) * anchorsPerHead;
}

void Yolov4CfgWriter::net_section()
{
  const std::size_t maxBatches = iterations();

  stream << "# The YOLO v4 detector of the darknet project, written for the "
         << classes << " classes\n"
         << "# of the exported annotations project.\n"
         << "# https://github.com/yuriysydor1991/darknetxx\n"
         << "#\n"
         << "#   darknet detector train data/obj.data cfg/yolov4-obj.cfg "
            "yolov4.conv.137 -map\n"
         << "#\n"
         << "# Raise the subdivisions below, or lower the width and the height "
            "to any\n"
         << "# other multiple of 32, when the training runs out of the video "
            "memory. The\n"
         << "# anchors are the COCO ones: darknet detector calc_anchors fits "
            "them to a\n"
         << "# dataset of one's own.\n";

  stream << "\n[net]\n"
         << "# Testing\n"
         << "#batch=1\n"
         << "#subdivisions=1\n"
         << "# Training\n"
         << "batch=" << batch << "\n"
         << "subdivisions=" << subdivisions << "\n"
         << "width=" << netWidth << "\n"
         << "height=" << netHeight << "\n"
         << "channels=3\n"
         << "momentum=0.949\n"
         << "decay=0.0005\n"
         << "angle=0\n"
         << "saturation=1.5\n"
         << "exposure=1.5\n"
         << "hue=.1\n"
         << "\n"
         << "learning_rate=0.001\n"
         << "burn_in=1000\n"
         << "max_batches=" << maxBatches << "\n"
         << "policy=steps\n"
         << "steps=" << (maxBatches * 80 / 100) << ","
         << (maxBatches * 90 / 100) << "\n"
         << "scales=.1,.1\n"
         << "\n"
         << "mosaic=1\n";
}

Yolov4CfgWriter::MergeLayers Yolov4CfgWriter::backbone()
{
  stream << "\n# The CSPDarknet53 backbone\n";

  conv(32, 3, 1, mish);

  csp_stage(64, 64, 32, 1, false);
  csp_stage(128, 64, 64, 2, false);

  const std::size_t stride8 = csp_stage(256, 128, 128, 8, false);
  const std::size_t stride16 = csp_stage(512, 256, 256, 8, false);

  csp_stage(1024, 512, 512, 4, true);

  return {stride16, stride8};
}

std::size_t Yolov4CfgWriter::csp_stage(std::size_t filters, std::size_t hidden,
                                       std::size_t middle, std::size_t blocks,
                                       bool last)
{
  conv(filters, 3, 2, mish);  // halves the resolution of the stage

  conv(hidden, 1, 1, mish);  // the half of the stage that skips the blocks
  route("-2");
  conv(hidden, 1, 1, mish);

  for (std::size_t block = 0; block < blocks; ++block) {
    conv(middle, 1, 1, mish);
    conv(hidden, 3, 1, mish);
    shortcut();
  }

  conv(hidden, 1, 1, mish);
  route("-1,-" + std::to_string(blocks * 3 + 4));  // the two halves back

  // the last stage of the backbone is the one the pre-trained weights end at,
  // so darknet is told to hold it still over the first iterations
  conv(filters, 1, 1, mish, last ? "stopbackward=800" : std::string{});

  return layers - 1;
}

void Yolov4CfgWriter::neck(const MergeLayers& merges)
{
  stream << "\n# The SPP and PANet neck\n";

  conv(512, 1, 1, leaky);
  conv(1024, 3, 1, leaky);
  conv(512, 1, 1, leaky);

  spp();

  conv(512, 1, 1, leaky);
  conv(1024, 3, 1, leaky);
  conv(512, 1, 1, leaky);

  upsample_merge(256, merges.first);
  conv_set(256);

  upsample_merge(128, merges.second);
  conv_set(128);
}

void Yolov4CfgWriter::spp()
{
  maxpool(5);
  route("-2");
  maxpool(9);
  route("-4");
  maxpool(13);
  route("-1,-3,-5,-6");
}

void Yolov4CfgWriter::upsample_merge(std::size_t filters,
                                     std::size_t backboneLayer)
{
  conv(filters, 1, 1, leaky);

  section("upsample");
  stream << "stride=2\n";

  route(std::to_string(backboneLayer));
  conv(filters, 1, 1, leaky);
  route("-1,-3");
}

void Yolov4CfgWriter::conv_set(std::size_t filters)
{
  conv(filters, 1, 1, leaky);
  conv(filters * 2, 3, 1, leaky);
  conv(filters, 1, 1, leaky);
  conv(filters * 2, 3, 1, leaky);
  conv(filters, 1, 1, leaky);
}

void Yolov4CfgWriter::heads()
{
  stream << "\n# The detectors, the finest resolution one first\n";

  head(128, "0,1,2", "1.2", false);

  route("-4");
  conv(256, 3, 2, leaky);
  route("-1,-16");
  conv_set(256);
  head(256, "3,4,5", "1.1", false);

  route("-4");
  conv(512, 3, 2, leaky);
  route("-1,-37");
  conv_set(512);
  head(512, "6,7,8", "1.05", true);
}

void Yolov4CfgWriter::head(std::size_t filters, const std::string& mask,
                           const std::string& scaleXY, bool multiscale)
{
  conv(filters * 2, 3, 1, leaky);
  conv(yolo_filters(), 1, 1, linear);

  section("yolo");
  stream << "mask=" << mask << "\n"
         << "anchors=" << anchors << "\n"
         << "classes=" << classes << "\n"
         << "num=9\n"
         << "jitter=.3\n"
         << "ignore_thresh=.7\n"
         << "truth_thresh=1\n"
         << "scale_x_y=" << scaleXY << "\n"
         << "iou_thresh=0.213\n"
         << "cls_normalizer=1.0\n"
         << "iou_normalizer=0.07\n"
         << "iou_loss=ciou\n"
         << "nms_kind=greedynms\n"
         << "beta_nms=0.6\n"
         << "max_delta=5\n";

  if (multiscale) {
    // darknet takes the multi scale training off the last of the detectors
    stream << "random=1\n";
  }
}

void Yolov4CfgWriter::conv(std::size_t filters, std::size_t size,
                           std::size_t stride, const std::string& activation,
                           const std::string& extra)
{
  section("convolutional");

  // the detectors, the linear ones, are the only convolutions darknet is not
  // asked to normalise, since they carry the coordinates and the classes out
  if (activation != linear) {
    stream << "batch_normalize=1\n";
  }

  stream << "filters=" << filters << "\n"
         << "size=" << size << "\n"
         << "stride=" << stride << "\n"
         << "pad=1\n"
         << "activation=" << activation << "\n";

  if (!extra.empty()) {
    stream << extra << "\n";
  }
}

void Yolov4CfgWriter::route(const std::string& layersList)
{
  section("route");
  stream << "layers=" << layersList << "\n";
}

void Yolov4CfgWriter::shortcut()
{
  section("shortcut");
  stream << "from=-3\n"
         << "activation=linear\n";
}

void Yolov4CfgWriter::maxpool(std::size_t size)
{
  section("maxpool");
  stream << "stride=1\n"
         << "size=" << size << "\n";
}

void Yolov4CfgWriter::section(const std::string& name)
{
  stream << "\n[" << name << "]\n";

  ++layers;
}

}  // namespace iannotator::exporters
