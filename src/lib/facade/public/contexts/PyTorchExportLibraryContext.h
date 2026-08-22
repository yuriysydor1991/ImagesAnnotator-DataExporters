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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_PYTORCHEXPORTLIBRARYCONTEXT_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_PYTORCHEXPORTLIBRARYCONTEXT_CLASS_H

#include <memory>

#include "../ExportersAPI.h"
#include "../IImageCropperFacility.h"
#include "../LibraryContext.h"

namespace ImagesAnnotatorDataExporters011
{

/**
 * @brief The library context which writes the PyTorch Vision ImageFolder
 * layout of cropped out rectangles. Requires an IImageCropperFacility unless
 * the library was built with its own.
 *
 * Instantiating the class is what names the wanted dataset layout. The cropper
 * lives here rather than in LibraryContext because this is the one layout
 * which cuts pixels out, so no other export has to carry a slot it never
 * reads.
 *
 * Current file is a target for the library header installation.
 */
class IADE_API PyTorchExportLibraryContext : public LibraryContext
{
 public:
  using PyTorchExportLibraryContextPtr =
      std::shared_ptr<PyTorchExportLibraryContext>;

  /// @brief In: the image cropper instance, optional in a build which found
  /// OpenCV and mandatory in one which did not
  const IImageCropperFacilityPtr& get_cropper() const;
  void set_cropper(const IImageCropperFacilityPtr& newCropper);

 private:
  IImageCropperFacilityPtr cropper;
};

using PyTorchExportLibraryContextPtr =
    PyTorchExportLibraryContext::PyTorchExportLibraryContextPtr;

}  // namespace ImagesAnnotatorDataExporters011

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_PYTORCHEXPORTLIBRARYCONTEXT_CLASS_H
