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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_ULTRALYTICSSEGMENT2FOLDEREXPORTER_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_ULTRALYTICSSEGMENT2FOLDEREXPORTER_CLASS_H

#include <fstream>

#include "src/exporters/Ultralytics/Ultralytics2FolderExporter.h"

namespace iannotator::exporters
{

/**
 * @brief The exporter of the Ultralytics YOLO segmentation dataset.
 *
 * The label file line of a rectangle is its class index followed by the points
 * of the polygon which outlines the object. The format takes a polygon of any
 * three or more points; the mask of a rectangle annotation is the rectangle
 * outline itself, so the four corners are what the export writes.
 *
 * A mask is therefore only ever as tight as the drawn rectangle, which the
 * trained segmentation model reproduces. Reach for this layout when a
 * rectangle outline is mask enough, and annotate the objects with real
 * polygons when it is not.
 *
 * The eight numbers coincide with the ones UltralyticsObb2FolderExporter
 * writes, since both spell out the corners of one and the same rectangle. What
 * differs is the training task that reads them: a polygon of an arbitrary
 * point count here against the four corners of a rotated box there.
 */
class UltralyticsSegment2FolderExporter : public Ultralytics2FolderExporter
{
 public:
  virtual ~UltralyticsSegment2FolderExporter() = default;
  UltralyticsSegment2FolderExporter() = default;

 protected:
  virtual void express_rectangle_data(
      std::fstream& ftxt, const IndexType& index,
      const NormalizedRect& nrect) const override;
};

}  // namespace iannotator::exporters

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_ULTRALYTICSSEGMENT2FOLDEREXPORTER_CLASS_H
