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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_PASCALVOCEXPORTLIBRARYCONTEXT_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_PASCALVOCEXPORTLIBRARYCONTEXT_CLASS_H

#include <memory>

#include "ExportersAPI.h"
#include "LibraryContext.h"

namespace ImagesAnnotatorDataExporters011
{

/**
 * @brief The library context which writes the Pascal VOC dataset: the
 * JPEGImages directory, one Annotations XML descriptor per image over it and
 * the ImageSets/Main lists naming them.
 *
 * This is the devkit layout torchvision VOCDetection and the MMDetection
 * XMLDataset are pointed at, and the very files LabelImg saves its own work in,
 * so an exported project opens for correction in that annotator directly.
 *
 * A rectangle reaches the XML as the two corner points it was drawn between,
 * in the image own pixels, and its class name is written out as it stands
 * instead of as a position in the available annotations list, which is what
 * every other layout of this library writes.
 *
 * The class carries no data of its own: instantiating it is what names the
 * wanted dataset layout, everything else is inherited from LibraryContext.
 *
 * Current file is a target for the library header installation.
 */
class IADE_API PascalVocExportLibraryContext : public LibraryContext
{
 public:
  using PascalVocExportLibraryContextPtr =
      std::shared_ptr<PascalVocExportLibraryContext>;
};

using PascalVocExportLibraryContextPtr =
    PascalVocExportLibraryContext::PascalVocExportLibraryContextPtr;

}  // namespace ImagesAnnotatorDataExporters011

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_PASCALVOCEXPORTLIBRARYCONTEXT_CLASS_H
