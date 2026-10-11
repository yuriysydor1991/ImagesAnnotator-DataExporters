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

#include "src/exporters/Darknetxx/Dxxwjz2pAnnotationsWriter.h"

#include <cassert>
#include <cstddef>
#include <ostream>
#include <string>

#include "src/exporters/Darknetxx/Dxxwjz2NetworkWriter.h"

namespace iannotator::exporters
{

Dxxwjz2pAnnotationsWriter::Dxxwjz2pAnnotationsWriter(
    std::ostream& destination, const std::string& folderPath,
    const Images& folderImages)
    : stream{destination}, folder{folderPath}, images{folderImages}
{
}

void Dxxwjz2pAnnotationsWriter::write()
{
  stream << ",\n  \"annotations\": {\"ImagesAnnotator\": {\"version\": "
         << Dxxwjz2NetworkWriter::quote(annotatorVersion)
         << "}, \"annotations\": [";

  // the darknetxx groups the images by their folders, so a folder of no
  // image is not there
  if (images.empty()) {
    stream << "]}";
    return;
  }

  stream << "\n    {\"db\": {\"path\": " << Dxxwjz2NetworkWriter::quote(folder)
         << "}, \"annotations\": [";

  for (std::size_t index = 0; index < images.size(); ++index) {
    image(images[index], index == 0);
  }

  stream << "\n    ]}\n  ]}";
}

void Dxxwjz2pAnnotationsWriter::image(const Image& image, bool first)
{
  assert(image.record != nullptr);

  const ImageRecordPtr& ir = image.record;

  stream << (first ? "\n      {" : ",\n      {")
         << "\"rel_path\": " << Dxxwjz2NetworkWriter::quote(image.relPath)
         << ", \"width\": " << std::to_string(ir->iwidth)
         << ", \"height\": " << std::to_string(ir->iheight)
         << ", \"annotations\": [";

  for (std::size_t index = 0; index < ir->rects.size(); ++index) {
    const ImageRecordRectPtr& irr = ir->rects[index];

    assert(irr != nullptr);

    // the top left corner and the size of the box in the pixels of the image
    stream << (index == 0 ? "{" : ", {")
           << "\"name\": " << Dxxwjz2NetworkWriter::quote(irr->name)
           << ", \"rect\": {\"x\": " << std::to_string(irr->x)
           << ", \"y\": " << std::to_string(irr->y)
           << ", \"width\": " << std::to_string(irr->width)
           << ", \"height\": " << std::to_string(irr->height) << "}}";
  }

  stream << "]}";
}

}  // namespace iannotator::exporters
