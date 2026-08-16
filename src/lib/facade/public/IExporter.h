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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_IEXPORTER_ABSTRACT_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_IEXPORTER_ABSTRACT_CLASS_H

#include <memory>

#include "ExportersAPI.h"

namespace ImagesAnnotatorDataExporters011
{

/**
 * @brief Only declared here, since the LibraryContext holds an IExporterPtr
 * of its own and its header is the one which includes this file.
 */
class IADE_API LibraryContext;
using LibraryContextPtr = std::shared_ptr<LibraryContext>;

/**
 * @brief The abstract class to define the interface for the all available
 * annotation data exporters.
 *
 * Current file is a target for the library header installation.
 */
class IADE_API IExporter
{
 public:
  using IExporterPtr = std::shared_ptr<IExporter>;

  virtual ~IExporter() = default;
  IExporter() = default;

  /**
   * @brief Writes the database named by the context out in the layout this
   * exporter implements, skipping the records it cannot process.
   *
   * @param ectx The filled library context. Both its export path and its
   * database provider are mandatory. The layout the context type names is not
   * looked at here: the layout written is the one of this very exporter. An
   * implementation must not keep the context beyond the call, since a context
   * holding this exporter back would close a pointer cycle.
   *
   * @return Returns true when the export as a whole ran through.
   */
  virtual bool export_db(LibraryContextPtr ectx) = 0;
};

using IExporterPtr = IExporter::IExporterPtr;

}  // namespace ImagesAnnotatorDataExporters011

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_IEXPORTER_ABSTRACT_CLASS_H
