#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBFACTORY_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBFACTORY_CLASS_H

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <functional>
#include <memory>

#include "IADataExportersContext.h"
#include "IADataExportersLib.h"
#include "IExporter.h"
#include "IImageCropperFacility.h"
#include "contexts/CocoExportContext.h"
#include "contexts/CreateMLExportContext.h"
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
 * @brief Pure abstract stand-in for the real LibFactory, so that every
 * factory method can be mocked with an (override) MOCK_METHOD.
 */
class LibFactorySynthParent
{
 public:
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
  using IExporterPtr = ImagesAnnotatorDataExporters012::IExporterPtr;
  using IImageCropperFacilityPtr =
      ImagesAnnotatorDataExporters012::IImageCropperFacilityPtr;

  virtual ~LibFactorySynthParent() = default;
  LibFactorySynthParent() = default;

  virtual IADataExportersLibPtr create_default_lib() = 0;
  virtual PlainTxtExportContextPtr create_plain_txt_library_context() = 0;
  virtual Yolo4ExportContextPtr create_yolo4_library_context() = 0;
  virtual UltralyticsDetectExportContextPtr
  create_ultralytics_detect_library_context() = 0;
  virtual UltralyticsObbExportContextPtr
  create_ultralytics_obb_library_context() = 0;
  virtual UltralyticsSegmentExportContextPtr
  create_ultralytics_segment_library_context() = 0;
  virtual CocoExportContextPtr create_coco_library_context() = 0;
  virtual PascalVocExportContextPtr create_pascal_voc_library_context() = 0;
  virtual CreateMLExportContextPtr create_createml_library_context() = 0;
  virtual PyTorchExportContextPtr create_pytorch_library_context() = 0;
  virtual IADataExportersLibPtr create_appropriate_lib(
      IADataExportersContextPtr ctx) = 0;
  virtual IExporterPtr create_exporter(
      const IADataExportersContextPtr& ctx) = 0;
  virtual IImageCropperFacilityPtr create_image_cropper() = 0;
};

class LibFactory : public LibFactorySynthParent
{
 public:
  using LibFactoryPtr = std::shared_ptr<LibFactory>;

  virtual ~LibFactory() = default;
  LibFactory()
  {
    if (onMockCreate) {
      onMockCreate(*this);
    }
  }

  inline static std::function<void(LibFactory& instance)> onMockCreate;

  MOCK_METHOD(IADataExportersLibPtr, create_default_lib, (), (override));
  MOCK_METHOD(PlainTxtExportContextPtr, create_plain_txt_library_context, (),
              (override));
  MOCK_METHOD(Yolo4ExportContextPtr, create_yolo4_library_context, (),
              (override));
  MOCK_METHOD(UltralyticsDetectExportContextPtr,
              create_ultralytics_detect_library_context, (), (override));
  MOCK_METHOD(UltralyticsObbExportContextPtr,
              create_ultralytics_obb_library_context, (), (override));
  MOCK_METHOD(UltralyticsSegmentExportContextPtr,
              create_ultralytics_segment_library_context, (), (override));
  MOCK_METHOD(CocoExportContextPtr, create_coco_library_context, (),
              (override));
  MOCK_METHOD(PascalVocExportContextPtr, create_pascal_voc_library_context, (),
              (override));
  MOCK_METHOD(CreateMLExportContextPtr, create_createml_library_context, (),
              (override));
  MOCK_METHOD(PyTorchExportContextPtr, create_pytorch_library_context, (),
              (override));
  MOCK_METHOD(IADataExportersLibPtr, create_appropriate_lib,
              (IADataExportersContextPtr ctx), (override));
  MOCK_METHOD(IExporterPtr, create_exporter,
              (const IADataExportersContextPtr& ctx), (override));
  MOCK_METHOD(IImageCropperFacilityPtr, create_image_cropper, (), (override));

  inline static LibFactoryPtr create_factory()
  {
    return std::make_shared<LibFactory>();
  }
};

using LibFactoryPtr = LibFactory::LibFactoryPtr;

}  // namespace iade0impl

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBFACTORY_CLASS_H
