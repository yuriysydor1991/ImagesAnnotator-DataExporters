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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBRARYFACADE_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBRARYFACADE_CLASS_H

#include <string>

#include "ExportContext.h"
#include "ExportFormat.h"
#include "ExportersAPI.h"
#include "IExporter.h"
#include "IImageCropperFacility.h"
#include "ILib.h"
#include "LibraryContext.h"

namespace ImagesAnnotatorDataExporters011
{

/**
 * @brief The facade interface class of the library.
 *
 * This is the only entry point a consuming project needs: every concrete
 * implementation class of the library stays hidden behind the abstract
 * interfaces returned from here.
 *
 * @code
 * namespace iadd = ImagesAnnotatorDataDrivers011;
 * namespace iade = ImagesAnnotatorDataExporters011;
 *
 * auto ectx = iade::LibraryFacade::create_export_context();
 * ectx->dbProvider = iadd::LibraryFacade::open_annotations_db("project.json");
 * ectx->export_path = "/tmp/yolo-dataset";
 *
 * auto exporter =
 *     iade::LibraryFacade::create_exporter(iade::ExportFormat::Yolo42Folder);
 *
 * return exporter != nullptr && exporter->export_db(ectx);
 * @endcode
 *
 * Current file is a target for the library header installation.
 */
class IADE_API LibraryFacade
{
 public:
  virtual ~LibraryFacade() = default;
  LibraryFacade() = default;

  /**
   * @brief Factory method to create an empty library context.
   *
   * @return Returns a new LibraryContext class instance.
   */
  static LibraryContextPtr create_library_context();

  /**
   * @brief Creates the default library implementation.
   *
   * @return Returns the default library implementation. Currently returns
   * the LibMain class instance.
   */
  static ILibPtr create_default_lib();

  /**
   * @brief Creates and returns the appropriate ILib instance.
   *
   * @param ctx Filled LibraryContext to check for an appropriate ILib
   * instance.
   *
   * @return Returns a new ILib descendant or a nullptr in case of any error.
   */
  static ILibPtr create_library(LibraryContextPtr ctx);

  /**
   * @brief Factory method to create an empty export context.
   *
   * @return Returns a new ExportContext class instance.
   */
  static ExportContextPtr create_export_context();

  /**
   * @brief Creates the exporter implementing the given dataset layout.
   *
   * @param format The wanted export format.
   *
   * @return Returns a new IExporter descendant, or a nullptr for an unknown
   * format.
   */
  static IExporterPtr create_exporter(const ExportFormat& format);

  /**
   * @brief Factory method to create the image cropper the library ships
   * itself.
   *
   * The library decodes no image format of its own, which is why
   * ExportContext::cropper exists: the PyTorch Vision export asks its consumer
   * to cut the rectangles out. When this build of the library found OpenCV it
   * is able to do that itself, and this method hands out such a cropper.
   *
   * Assigning it to ExportContext::cropper is optional. An export left with an
   * empty cropper slot falls back to this very cropper on its own, so a
   * consumer with nothing better to offer may simply leave the field alone. A
   * cropper the consumer does assign always wins over this one.
   *
   * @return Returns a new cropper, or a nullptr when the library was built
   * without OpenCV. A nullptr means the export needs a cropper of your own.
   */
  static IImageCropperFacilityPtr create_image_cropper();

  /**
   * @brief Reports the version of the library binary being used.
   *
   * @return Returns the library build version string.
   */
  static std::string library_version();
};

}  // namespace ImagesAnnotatorDataExporters011

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBRARYFACADE_CLASS_H
