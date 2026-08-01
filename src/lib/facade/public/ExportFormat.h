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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_EXPORTFORMAT_ENUM_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_EXPORTFORMAT_ENUM_H

/**
 * @brief The public interface namespace of the ImagesAnnotator data exporters
 * library.
 *
 * The namespace name carries the library major and minor version numbers
 * (0.11 gives the 011 suffix) so that several library versions may coexist
 * inside a single translation unit. Alias it downstream, for example
 * `namespace iade = ImagesAnnotatorDataExporters011;`.
 *
 * Current file is a target for the library header installation.
 */
namespace ImagesAnnotatorDataExporters011
{

/**
 * @brief The dataset layouts an annotations database may be written out as.
 *
 * Current file is a target for the library header installation.
 */
enum class ExportFormat
{
  /// @brief One `<annotation-name>.txt` file per annotation name.
  PlainTxt2Folder,

  /// @brief The YOLO v4 (darknet) training directory.
  Yolo42Folder,

  /// @brief The PyTorch Vision ImageFolder layout of cropped out rectangles.
  /// Requires an IImageCropperFacility in the export context.
  PyTorchVisionFolder
};

}  // namespace ImagesAnnotatorDataExporters011

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_EXPORTFORMAT_ENUM_H
