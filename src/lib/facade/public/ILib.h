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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_ILIB_ABSTRACT_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_ILIB_ABSTRACT_CLASS_H

#include <memory>

#include "ExportersAPI.h"
#include "LibraryContext.h"

/**
 * @brief The implementation part of the library.
 * Hides all the guts of particular implementation code.
 */
namespace ImagesAnnotatorDataExporters011
{

/**
 * @brief An abstract class to define an interface for the library
 * implementation variants.
 *
 * The perform_export method is the single shot entry point of the library: it
 * takes a filled context, builds the exporter of the layout that context
 * stands for and runs that exporter over the database it points at. Projects
 * that need a finer grained control should rather build the exporter directly
 * through the LibraryFacade factory methods.
 *
 * Current file is a target for the library header installation.
 */
class IADE_API ILib
{
 public:
  using ILibPtr = std::shared_ptr<ILib>;

  virtual ~ILib() = default;
  ILib() = default;

  /**
   * @brief The library interface method which every ILib descendant
   * must implement in order to provide it's functionality.
   *
   * @param ctx A filled LibraryContext descendant of the wanted dataset
   * layout. On success the ctx->exporter field receives the exporter instance
   * the export was performed with.
   *
   * @return Should return a true value on the success and false
   * in case of any error.
   */
  virtual bool perform_export(LibraryContextPtr ctx) = 0;
};

using ILibPtr = ILib::ILibPtr;

}  // namespace ImagesAnnotatorDataExporters011

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_ILIB_ABSTRACT_CLASS_H
