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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_IADATAEXPORTERSFACADE_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_IADATAEXPORTERSFACADE_CLASS_H

#include <memory>
#include <string>

#include "ExportersAPI.h"
#include "IADataExportersContext.h"
#include "IADataExportersLib.h"
#include "IExporter.h"
#include "IImageCropperFacility.h"
#include "contexts/CocoExportContext.h"
#include "contexts/CreateMLExportContext.h"
#include "contexts/Dxxwjz2ExportContext.h"
#include "contexts/Dxxwjz2pExportContext.h"
#include "contexts/Dxxwjz3ExportContext.h"
#include "contexts/Dxxwjz3pExportContext.h"
#include "contexts/PascalVocExportContext.h"
#include "contexts/PlainTxtExportContext.h"
#include "contexts/PyTorchExportContext.h"
#include "contexts/UltralyticsDetectExportContext.h"
#include "contexts/UltralyticsObbExportContext.h"
#include "contexts/UltralyticsSegmentExportContext.h"
#include "contexts/Yolo4ExportContext.h"

/**
 * @brief The logging subsystem interface every project built from the
 * cpp-app-template shares. The definition compiled into this library is token
 * identical to the one of the project which uses it, so this installed header
 * only forward declares it and never collides with the consumer own
 * logger::ILogger definition.
 */
namespace logger
{
class ILogger;
}  // namespace logger

namespace ImagesAnnotatorDataExporters013
{

/**
 * @brief The facade interface class of the library.
 *
 * This is the only entry point a consuming project needs: every concrete
 * implementation class of the library stays hidden behind the abstract
 * interfaces returned from here.
 *
 * @code
 * namespace iadd = ImagesAnnotatorDataDrivers013;
 * namespace iade = ImagesAnnotatorDataExporters013;
 *
 * auto db = iadd::IADataDriversFacade::open_annotations_db("project.json");
 *
 * auto ctx = iade::IADataExportersFacade::create_yolo4_library_context();
 * ctx->set_db_provider(db);
 * ctx->set_export_path("/tmp/yolo-dataset");
 *
 * auto exporter = iade::IADataExportersFacade::create_exporter(ctx);
 *
 * return exporter != nullptr && exporter->export_db(ctx);
 * @endcode
 *
 * Current file is a target for the library header installation.
 */
class IADE_API IADataExportersFacade
{
 public:
  virtual ~IADataExportersFacade() = default;
  IADataExportersFacade() = default;

  /**
   * @brief Factory method to create an empty library context of the plain
   * text dataset layout.
   *
   * @return Returns a new PlainTxtExportContext class instance.
   */
  static PlainTxtExportContextPtr create_plain_txt_library_context();

  /**
   * @brief Factory method to create an empty library context of the YOLO v4
   * dataset layout.
   *
   * @return Returns a new Yolo4ExportContext class instance.
   */
  static Yolo4ExportContextPtr create_yolo4_library_context();

  /**
   * @brief Factory method to create an empty library context of the
   * Ultralytics YOLO detection dataset layout.
   *
   * @return Returns a new UltralyticsDetectExportContext class
   * instance.
   */
  static UltralyticsDetectExportContextPtr
  create_ultralytics_detect_library_context();

  /**
   * @brief Factory method to create an empty library context of the
   * Ultralytics YOLO oriented bounding box dataset layout.
   *
   * @return Returns a new UltralyticsObbExportContext class instance.
   */
  static UltralyticsObbExportContextPtr
  create_ultralytics_obb_library_context();

  /**
   * @brief Factory method to create an empty library context of the
   * Ultralytics YOLO segmentation dataset layout.
   *
   * @return Returns a new UltralyticsSegmentExportContext class
   * instance.
   */
  static UltralyticsSegmentExportContextPtr
  create_ultralytics_segment_library_context();

  /**
   * @brief Factory method to create an empty library context of the COCO
   * object detection dataset layout.
   *
   * @return Returns a new CocoExportContext class instance.
   */
  static CocoExportContextPtr create_coco_library_context();

  /**
   * @brief Factory method to create an empty library context of the Pascal VOC
   * dataset layout.
   *
   * @return Returns a new PascalVocExportContext class instance.
   */
  static PascalVocExportContextPtr create_pascal_voc_library_context();

  /**
   * @brief Factory method to create an empty library context of the Create ML
   * object detection dataset layout.
   *
   * @return Returns a new CreateMLExportContext class instance.
   */
  static CreateMLExportContextPtr create_createml_library_context();

  /**
   * @brief Factory method to create an empty library context of the
   * darknetxx training directory layout, the one carrying the dxxwjz2 network
   * to train.
   *
   * @return Returns a new Dxxwjz2ExportContext class instance.
   */
  static Dxxwjz2ExportContextPtr create_dxxwjz2_library_context();

  /**
   * @brief Factory method to create an empty library context of the
   * darknetxx training directory layout, the one carrying the dxxwjz3 network
   * to train.
   *
   * @return Returns a new Dxxwjz3ExportContext class instance.
   */
  static Dxxwjz3ExportContextPtr create_dxxwjz3_library_context();

  /**
   * @brief Factory method to create an empty library context of the
   * darknetxx dxxwjz2p training project layout, the project file beside
   * the images folder.
   *
   * @return Returns a new Dxxwjz2pExportContext class instance.
   */
  static Dxxwjz2pExportContextPtr create_dxxwjz2p_library_context();

  /**
   * @brief Factory method to create an empty library context of the
   * darknetxx dxxwjz3p training project layout, the project file beside
   * the images folder.
   *
   * @return Returns a new Dxxwjz3pExportContext class instance.
   */
  static Dxxwjz3pExportContextPtr create_dxxwjz3p_library_context();

  /**
   * @brief Factory method to create an empty library context of the PyTorch
   * Vision dataset layout, the one carrying the image cropper.
   *
   * @return Returns a new PyTorchExportContext class instance.
   */
  static PyTorchExportContextPtr create_pytorch_library_context();

  /**
   * @brief Creates the default library implementation.
   *
   * @return Returns the default library implementation. Currently returns
   * the LibMain class instance.
   */
  static IADataExportersLibPtr create_default_lib();

  /**
   * @brief Creates and returns the appropriate IADataExportersLib instance.
   *
   * @param ctx Filled IADataExportersContext to check for an appropriate
   * IADataExportersLib instance.
   *
   * @return Returns a new IADataExportersLib descendant or a nullptr in case of
   * any error.
   */
  static IADataExportersLibPtr create_library(IADataExportersContextPtr ctx);

  /**
   * @brief Creates the exporter implementing the dataset layout of the given
   * context.
   *
   * @param ctx The IADataExportersContext descendant naming the wanted layout.
   *
   * @return Returns a new IExporter descendant, or a nullptr for an empty
   * context.
   */
  static IExporterPtr create_exporter(const IADataExportersContextPtr& ctx);

  /**
   * @brief Factory method to create the image cropper the library ships
   * itself.
   *
   * The library decodes no image format of its own, which is why
   * PyTorchExportContext::set_cropper() exists: the PyTorch Vision
   * export asks its consumer to cut the rectangles out. When this build of the
   * library found OpenCV it is able to do that itself, and this method hands
   * out such a cropper.
   *
   * Handing it over to that setter is optional. An export left with an empty
   * cropper slot falls back to this very cropper on its own, so a consumer
   * with nothing better to offer may simply leave the slot alone. A cropper
   * the consumer does set always wins over this one.
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

  /**
   * @brief Hands the logger instance of the project which uses this library
   * over to the library logging subsystem, so the whole binary logs into the
   * very same logger instead of the library keeping a silent own one.
   *
   * Call it before the first use of the library, usually right after the own
   * logging initialization. Any logger::ILogger implementation is accepted: a
   * project built from the cpp-app-template shares its default logger by
   * passing the LOG_REAL_LOGGER() value.
   *
   * @param realLogger The logger instance to adopt. A nullptr is ignored and
   * the previously adopted or the library own instance stays.
   */
  static void accept_real_logger(
      const std::shared_ptr<logger::ILogger>& realLogger);
};

}  // namespace ImagesAnnotatorDataExporters013

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_IADATAEXPORTERSFACADE_CLASS_H
