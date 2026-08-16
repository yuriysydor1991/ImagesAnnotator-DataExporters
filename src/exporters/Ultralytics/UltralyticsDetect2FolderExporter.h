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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_ULTRALYTICSDETECT2FOLDEREXPORTER_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_ULTRALYTICSDETECT2FOLDEREXPORTER_CLASS_H

#include <fstream>

#include "src/exporters/Ultralytics/Ultralytics2FolderExporter.h"

namespace iannotator::exporters
{

/**
 * @brief The exporter of the Ultralytics YOLO detection dataset.
 *
 * The label file line of a rectangle is its class index followed by the
 * centre and the size of the box, the very four numbers the darknet layout
 * writes as well. Everything around that line - the data.yaml descriptor and
 * the images/train plus labels/train directories - is the layout of the
 * Ultralytics2FolderExporter base.
 */
class UltralyticsDetect2FolderExporter : public Ultralytics2FolderExporter
{
 public:
  virtual ~UltralyticsDetect2FolderExporter() = default;
  UltralyticsDetect2FolderExporter() = default;

 protected:
  virtual void express_rectangle_data(
      std::fstream& ftxt, const IndexType& index,
      const NormalizedRect& nrect) const override;
};

}  // namespace iannotator::exporters

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_ULTRALYTICSDETECT2FOLDEREXPORTER_CLASS_H
