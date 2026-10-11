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
  using IADataExportersLibPtr =
      ImagesAnnotatorDataExporters012::IADataExportersLibPtr;
  using IADataExportersContextPtr =
      ImagesAnnotatorDataExporters012::IADataExportersContextPtr;
  using IADataExportersContext =
      ImagesAnnotatorDataExporters012::IADataExportersContext;
  using PlainTxtExportContextPtr =
      ImagesAnnotatorDataExporters012::PlainTxtExportContextPtr;
  using Yolo4ExportContextPtr =
      ImagesAnnotatorDataExporters012::Yolo4ExportContextPtr;
  using PyTorchExportContextPtr =
      ImagesAnnotatorDataExporters012::PyTorchExportContextPtr;
  using UltralyticsDetectExportContextPtr =
      ImagesAnnotatorDataExporters012::UltralyticsDetectExportContextPtr;
  using UltralyticsObbExportContextPtr =
      ImagesAnnotatorDataExporters012::UltralyticsObbExportContextPtr;
  using UltralyticsSegmentExportContextPtr =
      ImagesAnnotatorDataExporters012::UltralyticsSegmentExportContextPtr;
  using CocoExportContextPtr =
      ImagesAnnotatorDataExporters012::CocoExportContextPtr;
  using PascalVocExportContextPtr =
      ImagesAnnotatorDataExporters012::PascalVocExportContextPtr;
  using CreateMLExportContextPtr =
      ImagesAnnotatorDataExporters012::CreateMLExportContextPtr;
  using Dxxwjz2ExportContextPtr =
      ImagesAnnotatorDataExporters012::Dxxwjz2ExportContextPtr;
  using Dxxwjz3ExportContextPtr =
      ImagesAnnotatorDataExporters012::Dxxwjz3ExportContextPtr;
  using Dxxwjz2pExportContextPtr =
      ImagesAnnotatorDataExporters012::Dxxwjz2pExportContextPtr;
  using Dxxwjz3pExportContextPtr =
      ImagesAnnotatorDataExporters012::Dxxwjz3pExportContextPtr;
  using IExporterPtr = ImagesAnnotatorDataExporters012::IExporterPtr;
  using IImageCropperFacilityPtr =
      ImagesAnnotatorDataExporters012::IImageCropperFacilityPtr;

  virtual ~LibFactory() = default;
  LibFactory() = default;

  /**
   * @brief Creates the default library implementation. May be overriden
   * in descendants.
   *
   * @return Returns the default library implementation. Currently returns
   * the LibMain class instance.
   */
  virtual IADataExportersLibPtr create_default_lib();

  /**
   * @brief Creates an empty context of the plain text dataset layout.
   *
   * @return Returns an empty PlainTxtExportContext instance.
   */
  virtual PlainTxtExportContextPtr create_plain_txt_library_context();

  /**
   * @brief Creates an empty context of the YOLO v4 dataset layout.
   *
   * @return Returns an empty Yolo4ExportContext instance.
   */
  virtual Yolo4ExportContextPtr create_yolo4_library_context();

  /**
   * @brief Creates an empty context of the Ultralytics YOLO detection dataset
   * layout.
   *
   * @return Returns an empty UltralyticsDetectExportContext instance.
   */
  virtual UltralyticsDetectExportContextPtr
  create_ultralytics_detect_library_context();

  /**
   * @brief Creates an empty context of the Ultralytics YOLO oriented bounding
   * box dataset layout.
   *
   * @return Returns an empty UltralyticsObbExportContext instance.
   */
  virtual UltralyticsObbExportContextPtr
  create_ultralytics_obb_library_context();

  /**
   * @brief Creates an empty context of the Ultralytics YOLO segmentation
   * dataset layout.
   *
   * @return Returns an empty UltralyticsSegmentExportContext instance.
   */
  virtual UltralyticsSegmentExportContextPtr
  create_ultralytics_segment_library_context();

  /**
   * @brief Creates an empty context of the COCO object detection dataset
   * layout.
   *
   * @return Returns an empty CocoExportContext instance.
   */
  virtual CocoExportContextPtr create_coco_library_context();

  /**
   * @brief Creates an empty context of the Pascal VOC dataset layout.
   *
   * @return Returns an empty PascalVocExportContext instance.
   */
  virtual PascalVocExportContextPtr create_pascal_voc_library_context();

  /**
   * @brief Creates an empty context of the Create ML object detection dataset
   * layout.
   *
   * @return Returns an empty CreateMLExportContext instance.
   */
  virtual CreateMLExportContextPtr create_createml_library_context();

  /**
   * @brief Creates an empty context of the darknetxx training directory
   * layout.
   *
   * @return Returns an empty Dxxwjz2ExportContext instance.
   */
  virtual Dxxwjz2ExportContextPtr create_dxxwjz2_library_context();

  /**
   * @brief Creates an empty context of the darknetxx training directory
   * layout of the dxxwjz3 network.
   *
   * @return Returns an empty Dxxwjz3ExportContext instance.
   */
  virtual Dxxwjz3ExportContextPtr create_dxxwjz3_library_context();

  /**
   * @brief Creates an empty context of the darknetxx dxxwjz2p training
   * project layout.
   *
   * @return Returns an empty Dxxwjz2pExportContext instance.
   */
  virtual Dxxwjz2pExportContextPtr create_dxxwjz2p_library_context();

  /**
   * @brief Creates an empty context of the darknetxx dxxwjz3p training
   * project layout.
   *
   * @return Returns an empty Dxxwjz3pExportContext instance.
   */
  virtual Dxxwjz3pExportContextPtr create_dxxwjz3p_library_context();

  /**
   * @brief Creates an empty context of the PyTorch Vision dataset layout.
   *
   * @return Returns an empty PyTorchExportContext instance.
   */
  virtual PyTorchExportContextPtr create_pytorch_library_context();

  /**
   * @brief Creates appropriate library implementation instance
   * with accordance of provided data in the given context.
   *
   * @return Returns the appropriate library implementation if any
   * or a nullptr in case of any error. Currently returns the LibMain class
   * instance only.
   */
  virtual IADataExportersLibPtr create_appropriate_lib(
      IADataExportersContextPtr ctx);

  /**
   * @brief Creates the exporter implementing the dataset layout of the given
   * context.
   *
   * @param ctx The IADataExportersContext descendant naming the wanted layout.
   *
   * @return Returns a new exporter or a nullptr for an empty context.
   */
  virtual IExporterPtr create_exporter(const IADataExportersContextPtr& ctx);

  /**
   * @brief Creates the image cropper the library ships itself.
   *
   * @return Returns a new cropper when this build found OpenCV, and a nullptr
   * when it did not. A nullptr only means the consumer has to supply one of
   * its own through PyTorchExportContext::set_cropper().
   */
  virtual IImageCropperFacilityPtr create_image_cropper();

  static LibFactoryPtr create_factory();
};

using LibFactoryPtr = LibFactory::LibFactoryPtr;

}  // namespace iade0impl

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBFACTORY_CLASS_H
