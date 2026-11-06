/**
 * The "Simplified BSD License"
 *
 * Copyright (c) 2026, Yurii Sydor (yuriysydor1991@gmail.com)
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

#include "src/exporters/Ultralytics/Yolo26Segment2FolderExporter.h"

#include <algorithm>
#include <fstream>

namespace iannotator::exporters
{

// An Ultralytics release refuses a whole image over a label coordinate outside
// of the 0..1 range, so a point drawn over an edge is moved onto it
void Yolo26Segment2FolderExporter::express_annotation_data(
    std::fstream& ftxt, const IndexType& index, const ImageRecordPtr& ir,
    const ImageRecordRectPtr& irr,
    [[maybe_unused]] const NormalizedRect& nrect) const
{
  const double iwidth = toD(ir->iwidth);
  const double iheight = toD(ir->iheight);

  ftxt << index;

  for (const auto& [px, py] : irr->outline()) {
    ftxt << " " << std::clamp(px, 0.0, iwidth) / iwidth << " "
         << std::clamp(py, 0.0, iheight) / iheight;
  }

  ftxt << std::endl;
}

}  // namespace iannotator::exporters
