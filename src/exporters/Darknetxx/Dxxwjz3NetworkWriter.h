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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ3NETWORKWRITER_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ3NETWORKWRITER_CLASS_H

#include <ostream>
#include <string>
#include <vector>

#include "src/exporters/Darknetxx/Dxxwjz2NetworkWriter.h"
#include "src/exporters/Darknetxx/Dxxwjz3CfgJson.h"

namespace iannotator::exporters
{

/**
 * @brief The JSON document writer of the darknetxx network to train of the
 * dxxwjz3 file of the third version.
 *
 * The dxxwjz3 document of the darknetxx
 * (https://github.com/yuriysydor1991/darknetxx) is the dxxwjz2 one of nothing
 * but the JSON data: the network member keeps the JSON structure of the cfg
 * text instead of the text itself (see the Dxxwjz3CfgJson), the class names
 * are the array of the names of their indexes and the training record keeps
 * no iteration column, the loss of an index being the one of the iteration of
 * the index + 1. The network to train - of no weights - keeps no layer arrays
 * and no header, the very document the darknetxx converter makes of a cfg file
 * and of a class names file:
 *
 * @code
 * {
 *   "format": "dxxwjz3",
 *   "version": 3,
 *   "generator": "ImagesAnnotatorDataExporters-0.14 0.14.0",
 *   "network": {"net": {"batch": 64, "hue": 0.1}, "layers": [
 *     {"type": "convolutional", "options": {"filters": 32, "size": 3}},
 *     {"type": "yolo", "options": {"mask": [0,1,2], "classes": 2}}
 *   ], "classes": ["cat", "dog"]},
 *   "training": {"seconds": 0,
 *     "loss": [],
 *     "avg_loss": []
 *   }
 * }
 * @endcode
 *
 * The JSON keeps neither the comments nor the white space of the cfg text,
 * which the darknet reads no network of anyway.
 */
class Dxxwjz3NetworkWriter : public Dxxwjz2NetworkWriter
{
 public:
  /// @brief The value of the format key of the dxxwjz3 files.
  inline static const std::string formatName{"dxxwjz3"};
  /// @brief The value of the version key, the darknetxx readers refuse every
  /// other one.
  inline static const int formatVersion{3};
  /// @brief The file name extension the darknetxx tells the format by.
  inline static const std::string extension{".dxxwjz3"};

  /**
   * @param destination The stream the JSON text goes into.
   * @param cfgText The text of the network cfg file, the `[net]` section of
   * which comes first.
   * @param classNames The class names in the order of their indexes.
   */
  Dxxwjz3NetworkWriter(std::ostream& destination, const std::string& cfgText,
                       const std::vector<std::string>& classNames);
  virtual ~Dxxwjz3NetworkWriter() = default;

 protected:
  /// @brief Tells the JSON keeps the cfg text (see the Dxxwjz3CfgJson).
  virtual bool keeps_cfg() const override;

  virtual const std::string& format_name() const override;
  virtual int format_version() const override;

  /// @brief Writes the network member: the `net` options, the layers of the
  /// rest of the sections, a layer on a line, and the class names array.
  virtual void network() override;
  /// @brief Writes the empty training record of no iteration column.
  virtual void training() override;

 private:
  const Dxxwjz3CfgJson cfgJson;
};

}  // namespace iannotator::exporters

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ3NETWORKWRITER_CLASS_H
