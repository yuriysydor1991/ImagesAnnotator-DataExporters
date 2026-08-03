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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_OPENCVIMAGECROPPER_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_OPENCVIMAGECROPPER_CLASS_H

#include <string>

#include "src/lib/libmain/exporters/ExportersAliases.h"

namespace iannotator::exporters::croppers
{

/**
 * @brief The IImageCropperFacility implementation the library ships itself,
 * built on OpenCV.
 *
 * The library decodes no image format of its own, which is why the PyTorch
 * Vision export asks its consumer for a cropper. This class is the one
 * exception: when the build found OpenCV, the library is able to answer that
 * request itself, so a consumer without an imaging stack of its own still gets
 * that export. It is compiled only then - see create_builtin_cropper(), which
 * hands out a nullptr in a build without OpenCV.
 *
 * A cropper the consumer supplies through ExportContext::cropper always wins
 * over this one: a project that already decodes images its own way keeps doing
 * so, and this is only what fills an empty slot.
 *
 * Nothing declared here is installed. The class is reached through the
 * abstract IImageCropperFacility exactly like a consumer supplied cropper, so
 * no OpenCV type reaches a public header and no consuming project needs OpenCV
 * of its own.
 */
class OpenCVImageCropper : virtual public IImageCropperFacility
{
 public:
  virtual ~OpenCVImageCropper() = default;
  OpenCVImageCropper() = default;

  /**
   * @brief Cuts the given rectangle out of the given image with OpenCV and
   * writes it out as a PNG.
   *
   * The rectangle is clamped to the image bounds, so an annotation reaching
   * over an edge yields the overlapping part instead of a failure.
   *
   * @param ir The record naming the image to read.
   * @param irr The rectangle to cut out, in the image own pixel coordinates.
   * @param tofpath In-out: the wanted destination path. Rewritten to carry the
   * .png extension, and to a free -N suffixed name when the file is taken.
   *
   * @return Returns true when the cropped image was written out.
   */
  bool crop_out_2_fs(ImageRecordPtr ir, ImageRecordRectPtr irr,
                     std::string& tofpath) override;

  /**
   * @brief Produces a copy of this cropper.
   *
   * @return Returns a new usable instance. The class holds no decoding state
   * between the calls, so the copy is a plain new instance.
   */
  IImageCropperFacilityPtr clone() override;
};

}  // namespace iannotator::exporters::croppers

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_OPENCVIMAGECROPPER_CLASS_H
