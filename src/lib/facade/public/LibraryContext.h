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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBRARYCONTEXT_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBRARYCONTEXT_CLASS_H

#include <ImagesAnnotatorDataDrivers-0.11/IImagesPathsDBProvider.h>

#include <memory>
#include <string>

#include "ExportersAPI.h"
#include "IExporter.h"

namespace ImagesAnnotatorDataExporters011
{

/**
 * @brief The library context class designed to pass data in and out of
 * the library underlying implementation.
 *
 * The very same instance drives both entry points of the library: the one
 * shot ILib::perform_export and the IExporter::export_db of an exporter built
 * by hand through the LibraryFacade factory methods.
 *
 * The dataset layout to write is named by the descendant instantiated. What
 * every layout needs is held here; a descendant adds only what its own layout
 * asks for, the way PyTorchExportLibraryContext adds the image cropper. See
 * PlainTxtExportLibraryContext, Yolo4ExportLibraryContext and
 * PyTorchExportLibraryContext. This class itself names no layout, so an export
 * driven by it finds no exporter.
 *
 * Current file is a target for the library header installation.
 */
class IADE_API LibraryContext
{
 public:
  using LibraryContextPtr = std::shared_ptr<LibraryContext>;
  using IImagesPathsDBProviderPtr =
      ImagesAnnotatorDataDrivers011::IImagesPathsDBProviderPtr;

  virtual ~LibraryContext() = default;
  LibraryContext() = default;

  /// @brief In: the destination directory of the export, mandatory
  const std::string& get_export_path() const;
  void set_export_path(const std::string& newPath);

  /// @brief In: the annotations database to read the records out of, mandatory
  const IImagesPathsDBProviderPtr& get_db_provider() const;
  void set_db_provider(const IImagesPathsDBProviderPtr& newProvider);

  /// @brief Out: the exporter instance the last ILib::perform_export ran
  const IExporterPtr& get_exporter() const;
  void set_exporter(const IExporterPtr& newExporter);

 private:
  std::string export_path;
  IImagesPathsDBProviderPtr dbProvider;
  IExporterPtr exporter;
};

using LibraryContextPtr = LibraryContext::LibraryContextPtr;

}  // namespace ImagesAnnotatorDataExporters011

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBRARYCONTEXT_CLASS_H
