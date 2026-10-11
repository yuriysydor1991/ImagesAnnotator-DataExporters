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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ2PANNOTATIONSWRITER_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ2PANNOTATIONSWRITER_CLASS_H

#include <ostream>
#include <string>
#include <vector>

#include "src/exporters/ExportersAliases.h"

namespace iannotator::exporters
{

/**
 * @brief The JSON writer of the annotations member of the darknetxx training
 * projects, the dxxwjz2p and the dxxwjz3p ones.
 *
 * The darknetxx (https://github.com/yuriysydor1991/darknetxx) keeps the
 * annotations of the images of a training project the way the ImagesAnnotator
 * project file of its 0.12.0 structure does: the folders of the images of the
 * path relative to the project file (or absolute), the images of the path
 * relative to their folder and of their size, the boxes of the class name and
 * of the rect of the pixels of the image:
 *
 * @code
 *   "annotations": {"ImagesAnnotator": {"version": "0.12.0"}, "annotations": [
 *     {"db": {"path": "images"}, "annotations": [
 *       {"rel_path": "park.jpg", "width": 640, "height": 480, "annotations":
 *         [{"name": "dog", "rect": {"x": 200, "y": 130, "width": 48,
 *         "height": 52}}]}
 *     ]}
 *   ]}
 * @endcode
 *
 * The member is written the way the darknetxx writes it, an image on a line
 * (the box objects of an image on the line of it), after the training record
 * member: the text starts with the comma which separates it from that one.
 * The project of no image keeps the empty array of no folder.
 */
class Dxxwjz2pAnnotationsWriter
{
 public:
  /// @brief The version of the ImagesAnnotator project structure the
  /// darknetxx reads.
  inline static const std::string annotatorVersion{"0.12.0"};

  /// @brief An image of the project: the path relative to the folder of the
  /// images and the record of its size and its boxes.
  struct Image
  {
    std::string relPath;
    ImageRecordPtr record;
  };

  using Images = std::vector<Image>;

  /**
   * @param destination The stream the JSON text goes into.
   * @param folderPath The path of the folder of the images, relative to the
   * project file.
   * @param folderImages The images of the folder, the record of every one of
   * them of the size the boxes need.
   */
  Dxxwjz2pAnnotationsWriter(std::ostream& destination,
                            const std::string& folderPath,
                            const Images& folderImages);

  /// @brief Writes the whole member into the destination stream.
  void write();

 private:
  void image(const Image& image, bool first);

  std::ostream& stream;
  const std::string& folder;
  const Images& images;
};

}  // namespace iannotator::exporters

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ2PANNOTATIONSWRITER_CLASS_H
