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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_YOLOV4CFGWRITER_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_YOLOV4CFGWRITER_CLASS_H

#include <cstddef>
#include <ostream>
#include <string>
#include <utility>

namespace iannotator::exporters
{

/**
 * @brief The darknet neural network descriptor writer of the YOLO v4 detector.
 *
 * Writes the whole network of the darknet `cfg/yolov4-custom.cfg`
 * (https://github.com/yuriysydor1991/darknetxx): the CSPDarknet53 backbone of
 * five cross stage partial stages, the SPP and PANet neck and the three
 * detection heads - 162 layers, the network darknet trains out of the
 * `yolov4.conv.137` weights.
 *
 * The descriptor needs no editing by hand: every value that depends on the
 * exported project - the class count of the three `[yolo]` layers, the filters
 * count of the convolution in front of each of them and the iterations count -
 * is filled in out of the annotations database. The input resolution is a large
 * one, so that the detector still sees the small objects of a big photo.
 *
 * The network is emitted rather than kept as a text of its own, since the
 * backbone repeats one and the same residual block 23 times and the neck
 * repeats one and the same five convolutions block four times.
 */
class Yolov4CfgWriter
{
 public:
  /// @brief The network input width darknet resizes every image to.
  inline static const std::size_t netWidth{832};
  /// @brief The network input height darknet resizes every image to.
  inline static const std::size_t netHeight{832};
  /// @brief The images one training iteration is made of.
  inline static const std::size_t batch{64};
  /// @brief The parts a batch is loaded by. Raise it on an out of memory.
  inline static const std::size_t subdivisions{32};
  /// @brief The iterations count under which no training makes sense.
  inline static const std::size_t minIterations{6000};
  /// @brief The iterations count a single class of the dataset is given.
  inline static const std::size_t iterationsPerClass{2000};

  Yolov4CfgWriter(std::ostream& destination, std::size_t classesCount,
                  std::size_t imagesCount);

  /// @brief Writes the whole network descriptor into the destination stream.
  void write();

  /// @brief The iterations count the training is given: 2000 per class, never
  /// less than 6000 and never less than the images count of the dataset.
  std::size_t iterations() const;

  /// @brief The filters count of the convolution in front of a `[yolo]` layer:
  /// the four box coordinates, the objectness and the classes, per anchor.
  std::size_t yolo_filters() const;

 private:
  /// @brief The two backbone layers the neck merges into, the stride 16 one
  /// first, since the neck upsamples down to the stride 8 one.
  using MergeLayers = std::pair<std::size_t, std::size_t>;

  /// @brief The anchors the YOLO v4 authors have got on the COCO dataset.
  /// `darknet detector calc_anchors` fits them to a dataset of one's own.
  inline static const std::string anchors{
      "12,16, 19,36, 40,28, 36,75, 76,55, 72,146, 142,110, 192,243, 459,401"};

  void net_section();
  MergeLayers backbone();
  void neck(const MergeLayers& merges);
  void heads();

  std::size_t csp_stage(std::size_t filters, std::size_t hidden,
                        std::size_t middle, std::size_t blocks, bool last);
  void conv_set(std::size_t filters);
  void upsample_merge(std::size_t filters, std::size_t backboneLayer);
  void spp();
  void head(std::size_t filters, const std::string& mask,
            const std::string& scaleXY, bool multiscale);

  void section(const std::string& name);
  void conv(std::size_t filters, std::size_t size, std::size_t stride,
            const std::string& activation, const std::string& extra = {});
  void route(const std::string& layersList);
  void shortcut();
  void maxpool(std::size_t size);

  std::ostream& stream;
  const std::size_t classes;
  const std::size_t images;
  /// @brief The layers written so far, that is the index of the next one.
  std::size_t layers{0};
};

}  // namespace iannotator::exporters

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_YOLOV4CFGWRITER_CLASS_H
