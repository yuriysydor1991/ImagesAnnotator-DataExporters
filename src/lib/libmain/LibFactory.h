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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBFACTORY_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBFACTORY_CLASS_H

#include <memory>

#include "IExporter.h"
#include "IImageCropperFacility.h"
#include "ILib.h"
#include "LibraryContext.h"

namespace iade0impl
{

/**
 * The library main factory to operate with the implementation.
 * Create appropriate instances in accordance of provided data
 * by the given context or by simply calling appropriate method.
 */
class LibFactory
{
 public:
  using LibFactoryPtr = std::shared_ptr<LibFactory>;
  using ILibPtr = ImagesAnnotatorDataExporters011::ILibPtr;
  using LibraryContextPtr = ImagesAnnotatorDataExporters011::LibraryContextPtr;
  using LibraryContext = ImagesAnnotatorDataExporters011::LibraryContext;
  using IExporterPtr = ImagesAnnotatorDataExporters011::IExporterPtr;
  using IImageCropperFacilityPtr =
      ImagesAnnotatorDataExporters011::IImageCropperFacilityPtr;

  virtual ~LibFactory() = default;
  LibFactory() = default;

  /**
   * @brief Creates the default library implementation. May be overriden
   * in descendants.
   *
   * @return Returns the default library implementation. Currently returns
   * the LibMain class instance.
   */
  virtual ILibPtr create_default_lib();

  /**
   * @brief Creates the default context to use in libraries implementation
   * in order to provide all necessary data.
   *
   * @return Returns an empty PlainTxtExportLibraryContext instance.
   */
  virtual LibraryContextPtr create_default_context();

  /**
   * @brief Creates appropriate library implementation instance
   * with accordance of provided data in the given context.
   *
   * @return Returns the appropriate library implementation if any
   * or a nullptr in case of any error. Currently returns the LibMain class
   * instance only.
   */
  virtual ILibPtr create_appropriate_lib(LibraryContextPtr ctx);

  /**
   * @brief Creates the exporter implementing the dataset layout of the given
   * context.
   *
   * @param ctx The LibraryContext descendant naming the wanted layout.
   *
   * @return Returns a new exporter or a nullptr for an empty context.
   */
  virtual IExporterPtr create_exporter(const LibraryContextPtr& ctx);

  /**
   * @brief Creates the image cropper the library ships itself.
   *
   * @return Returns a new cropper when this build found OpenCV, and a nullptr
   * when it did not. A nullptr only means the consumer has to supply one of
   * its own through PyTorchExportLibraryContext::set_cropper().
   */
  virtual IImageCropperFacilityPtr create_image_cropper();

  static LibFactoryPtr create_factory();
};

using LibFactoryPtr = LibFactory::LibFactoryPtr;

}  // namespace iade0impl

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBFACTORY_CLASS_H
