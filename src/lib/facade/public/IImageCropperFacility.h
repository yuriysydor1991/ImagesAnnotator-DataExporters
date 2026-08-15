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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_IIMAGECROPPERFACILITY_ABSTRACT_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_IIMAGECROPPERFACILITY_ABSTRACT_CLASS_H

#include <ImagesAnnotatorDataDrivers-0.11/ImageRecord.h>
#include <ImagesAnnotatorDataDrivers-0.11/ImageRecordRect.h>

#include <memory>
#include <string>

#include "ExportersAPI.h"

namespace ImagesAnnotatorDataExporters011
{

/**
 * @brief The image cropping service the consuming project supplies for the
 * PyTorchExportLibraryContext export.
 *
 * The library decodes no image format of its own, so the one export that has
 * to cut a rectangle out of a picture asks its consumer to do it over
 * whatever imaging stack that project already links.
 *
 * Current file is a target for the library header installation.
 */
class IADE_API IImageCropperFacility
{
 public:
  using IImageCropperFacilityPtr = std::shared_ptr<IImageCropperFacility>;

  using ImageRecordPtr = ImagesAnnotatorDataDrivers011::ImageRecordPtr;
  using ImageRecordRectPtr = ImagesAnnotatorDataDrivers011::ImageRecordRectPtr;

  virtual ~IImageCropperFacility() = default;
  IImageCropperFacility() = default;

  /**
   * @brief Cuts the given rectangle out of the given image and writes the
   * result to the filesystem.
   *
   * @param ir The record naming the image to read through its
   * ImageRecord::get_full_path().
   * @param irr The rectangle to cut out, in the image own pixel coordinates.
   * @param tofpath In-out: the wanted destination file path, which an
   * implementation may rewrite, for example to append its own extension.
   *
   * @return Should return true when the cropped image was written out.
   */
  virtual bool crop_out_2_fs(ImageRecordPtr ir, ImageRecordRectPtr irr,
                             std::string& tofpath) = 0;

  /**
   * @brief Produces a copy sharing no mutable decoding state with this one.
   *
   * @return Should return a new usable instance.
   */
  virtual IImageCropperFacilityPtr clone() = 0;
};

using IImageCropperFacilityPtr =
    IImageCropperFacility::IImageCropperFacilityPtr;

}  // namespace ImagesAnnotatorDataExporters011

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_IIMAGECROPPERFACILITY_ABSTRACT_CLASS_H
