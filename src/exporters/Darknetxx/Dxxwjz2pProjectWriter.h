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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ2PPROJECTWRITER_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ2PPROJECTWRITER_CLASS_H

#include <ostream>
#include <string>
#include <vector>

#include "src/exporters/Darknetxx/Dxxwjz2NetworkWriter.h"
#include "src/exporters/Darknetxx/Dxxwjz2pAnnotationsWriter.h"

namespace iannotator::exporters
{

/**
 * @brief The JSON document writer of the darknetxx dxxwjz2p training project.
 *
 * The dxxwjz2p file of the darknetxx
 * (https://github.com/yuriysydor1991/darknetxx) is the dxxwjz2 document of the
 * YOLO v4 training project: the network, the weights and the training record
 * beside the annotations of the images, everything the training needs but the
 * images themselves. The project of no weights is the network to train from
 * scratch on the images of it:
 *
 * @code
 * {
 *   "format": "dxxwjz2p",
 *   "version": 1,
 *   "generator": "ImagesAnnotatorDataExporters-0.13 0.13.0",
 *   "network": {"cfg": "[net]\nbatch=64\n...", "classes": [
 *     {"index": 0, "name": "cat"}
 *   ]},
 *   "training": {"seconds": 0,
 *     "iteration": [],
 *     "loss": [],
 *     "avg_loss": []
 *   },
 *   "annotations": {"ImagesAnnotator": {"version": "0.12.0"}, "annotations": [
 *     {"db": {"path": "images"}, "annotations": [
 *       {"rel_path": "a.png", "width": 640, "height": 480, "annotations":
 * [...]}
 *     ]}
 *   ]}
 * }
 * @endcode
 *
 * The `darknetxx --train-cfg project.dxxwjz2p` trains it, and so does the
 * darknetxx-train-gui, which opens it.
 */
class Dxxwjz2pProjectWriter : public Dxxwjz2NetworkWriter
{
 public:
  /// @brief The value of the format key of the dxxwjz2p files.
  inline static const std::string formatName{"dxxwjz2p"};
  /// @brief The value of the version key, the darknetxx readers refuse every
  /// other one.
  inline static const int formatVersion{1};
  /// @brief The file name extension the darknetxx tells the format by.
  inline static const std::string extension{".dxxwjz2p"};

  /**
   * @param destination The stream the JSON text goes into.
   * @param cfgText The text of the network cfg file.
   * @param classNames The class names in the order of their indexes, the
   * names of the boxes included.
   * @param folderPath The path of the folder of the images, relative to the
   * project file.
   * @param folderImages The images of the folder.
   */
  Dxxwjz2pProjectWriter(std::ostream& destination, const std::string& cfgText,
                        const std::vector<std::string>& classNames,
                        const std::string& folderPath,
                        const Dxxwjz2pAnnotationsWriter::Images& folderImages);
  virtual ~Dxxwjz2pProjectWriter() = default;

 protected:
  virtual const std::string& format_name() const override;
  virtual int format_version() const override;

  /// @brief Writes the annotations member of the images after the training
  /// record one.
  virtual void members() override;

 private:
  const std::string& folder;
  const Dxxwjz2pAnnotationsWriter::Images& images;
};

}  // namespace iannotator::exporters

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ2PPROJECTWRITER_CLASS_H
