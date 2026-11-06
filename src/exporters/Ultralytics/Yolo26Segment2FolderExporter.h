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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_YOLO26SEGMENT2FOLDEREXPORTER_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_YOLO26SEGMENT2FOLDEREXPORTER_CLASS_H

#include <fstream>

#include "src/exporters/Ultralytics/UltralyticsSegment2FolderExporter.h"

namespace iannotator::exporters
{

/**
 * @brief The exporter of the Ultralytics YOLO26 segmentation dataset.
 *
 * The layout and the label file line are the Ultralytics segmentation ones,
 * which YOLO26 trains its -seg models on: the class index followed by at least
 * three normalised polygon points. The polygon is the outline of the
 * annotation - the polygon of a segment, the four corners of a rectangle - so
 * every line carries a mask, which a segmentation dataset of a line of a box
 * alone fails to load with.
 */
class Yolo26Segment2FolderExporter : public UltralyticsSegment2FolderExporter
{
 public:
  virtual ~Yolo26Segment2FolderExporter() = default;
  Yolo26Segment2FolderExporter() = default;

 protected:
  virtual void express_annotation_data(
      std::fstream& ftxt, const IndexType& index, const ImageRecordPtr& ir,
      const ImageRecordRectPtr& irr,
      const NormalizedRect& nrect) const override;
};

}  // namespace iannotator::exporters

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_YOLO26SEGMENT2FOLDEREXPORTER_CLASS_H
