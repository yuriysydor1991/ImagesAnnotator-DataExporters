#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBFACTORY_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBFACTORY_CLASS_H

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <functional>
#include <memory>

#include "IExporter.h"
#include "ILib.h"
#include "LibraryContext.h"
#include "contexts/PlainTxtExportLibraryContext.h"
#include "contexts/PyTorchExportLibraryContext.h"
#include "contexts/Yolo4ExportLibraryContext.h"

namespace iade0impl
{

/**
 * @brief Pure abstract stand-in for the real LibFactory, so that every
 * factory method can be mocked with an (override) MOCK_METHOD.
 */
class LibFactorySynthParent
{
 public:
  using ILibPtr = ImagesAnnotatorDataExporters011::ILibPtr;
  using LibraryContextPtr = ImagesAnnotatorDataExporters011::LibraryContextPtr;
  using LibraryContext = ImagesAnnotatorDataExporters011::LibraryContext;
  using PlainTxtExportLibraryContextPtr =
      ImagesAnnotatorDataExporters011::PlainTxtExportLibraryContextPtr;
  using Yolo4ExportLibraryContextPtr =
      ImagesAnnotatorDataExporters011::Yolo4ExportLibraryContextPtr;
  using PyTorchExportLibraryContextPtr =
      ImagesAnnotatorDataExporters011::PyTorchExportLibraryContextPtr;
  using IExporterPtr = ImagesAnnotatorDataExporters011::IExporterPtr;

  virtual ~LibFactorySynthParent() = default;
  LibFactorySynthParent() = default;

  virtual ILibPtr create_default_lib() = 0;
  virtual PlainTxtExportLibraryContextPtr
  create_plain_txt_library_context() = 0;
  virtual Yolo4ExportLibraryContextPtr create_yolo4_library_context() = 0;
  virtual PyTorchExportLibraryContextPtr create_pytorch_library_context() = 0;
  virtual ILibPtr create_appropriate_lib(LibraryContextPtr ctx) = 0;
  virtual IExporterPtr create_exporter(const LibraryContextPtr& ctx) = 0;
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

  MOCK_METHOD(ILibPtr, create_default_lib, (), (override));
  MOCK_METHOD(PlainTxtExportLibraryContextPtr, create_plain_txt_library_context,
              (), (override));
  MOCK_METHOD(Yolo4ExportLibraryContextPtr, create_yolo4_library_context, (),
              (override));
  MOCK_METHOD(PyTorchExportLibraryContextPtr, create_pytorch_library_context,
              (), (override));
  MOCK_METHOD(ILibPtr, create_appropriate_lib, (LibraryContextPtr ctx),
              (override));
  MOCK_METHOD(IExporterPtr, create_exporter, (const LibraryContextPtr& ctx),
              (override));

  inline static LibFactoryPtr create_factory()
  {
    return std::make_shared<LibFactory>();
  }
};

using LibFactoryPtr = LibFactory::LibFactoryPtr;

}  // namespace iade0impl

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBFACTORY_CLASS_H
