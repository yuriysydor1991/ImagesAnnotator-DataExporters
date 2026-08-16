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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_ULTRALYTICSOBBEXPORTLIBRARYCONTEXT_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_ULTRALYTICSOBBEXPORTLIBRARYCONTEXT_CLASS_H

#include <memory>

#include "ExportersAPI.h"
#include "LibraryContext.h"

namespace ImagesAnnotatorDataExporters011
{

/**
 * @brief The library context which writes the Ultralytics YOLO oriented
 * bounding box dataset: the data.yaml descriptor over the images/train and
 * labels/train directories, one `class x1 y1 x2 y2 x3 y3 x4 y4` line of the
 * four box corners per rectangle.
 *
 * The annotations database knows axis aligned rectangles only, so every box
 * written here carries the rotation angle of zero. An OBB model trained on it
 * still learns to detect the rotated instances of the very same objects.
 *
 * The class carries no data of its own: instantiating it is what names the
 * wanted dataset layout, everything else is inherited from LibraryContext.
 *
 * Current file is a target for the library header installation.
 */
class IADE_API UltralyticsObbExportLibraryContext : public LibraryContext
{
 public:
  using UltralyticsObbExportLibraryContextPtr =
      std::shared_ptr<UltralyticsObbExportLibraryContext>;
};

using UltralyticsObbExportLibraryContextPtr =
    UltralyticsObbExportLibraryContext::UltralyticsObbExportLibraryContextPtr;

}  // namespace ImagesAnnotatorDataExporters011

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_ULTRALYTICSOBBEXPORTLIBRARYCONTEXT_CLASS_H
