#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBFACTORY_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBFACTORY_CLASS_H

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <functional>
#include <memory>

#include "IADataExportersContext.h"
#include "IADataExportersLib.h"
#include "IExporter.h"
#include "contexts/PlainTxtExportContext.h"
#include "contexts/PyTorchExportContext.h"
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
  using IExporterPtr = ImagesAnnotatorDataExporters012::IExporterPtr;

  virtual ~LibFactorySynthParent() = default;
  LibFactorySynthParent() = default;

  virtual IADataExportersLibPtr create_default_lib() = 0;
  virtual PlainTxtExportContextPtr create_plain_txt_library_context() = 0;
  virtual Yolo4ExportContextPtr create_yolo4_library_context() = 0;
  virtual PyTorchExportContextPtr create_pytorch_library_context() = 0;
  virtual IADataExportersLibPtr create_appropriate_lib(
      IADataExportersContextPtr ctx) = 0;
  virtual IExporterPtr create_exporter(
      const IADataExportersContextPtr& ctx) = 0;
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
  MOCK_METHOD(PyTorchExportContextPtr, create_pytorch_library_context, (),
              (override));
  MOCK_METHOD(IADataExportersLibPtr, create_appropriate_lib,
              (IADataExportersContextPtr ctx), (override));
  MOCK_METHOD(IExporterPtr, create_exporter,
              (const IADataExportersContextPtr& ctx), (override));

  inline static LibFactoryPtr create_factory()
  {
    return std::make_shared<LibFactory>();
  }
};

using LibFactoryPtr = LibFactory::LibFactoryPtr;

}  // namespace iade0impl

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBFACTORY_CLASS_H
