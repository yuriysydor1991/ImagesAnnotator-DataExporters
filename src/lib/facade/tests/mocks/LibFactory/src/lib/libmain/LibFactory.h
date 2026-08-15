#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBFACTORY_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBFACTORY_CLASS_H

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <functional>
#include <memory>

#include "ExportContext.h"
#include "IExporter.h"
#include "IImageCropperFacility.h"
#include "ILib.h"
#include "LibraryContext.h"

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
  using ExportContextPtr = ImagesAnnotatorDataExporters011::ExportContextPtr;
  using IExporterPtr = ImagesAnnotatorDataExporters011::IExporterPtr;
  using IImageCropperFacilityPtr =
      ImagesAnnotatorDataExporters011::IImageCropperFacilityPtr;

  virtual ~LibFactorySynthParent() = default;
  LibFactorySynthParent() = default;

  virtual ILibPtr create_default_lib() = 0;
  virtual LibraryContextPtr create_default_context() = 0;
  virtual ILibPtr create_appropriate_lib(LibraryContextPtr ctx) = 0;
  virtual ExportContextPtr create_export_context() = 0;
  virtual IExporterPtr create_exporter(const LibraryContextPtr& ctx) = 0;
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

  MOCK_METHOD(ILibPtr, create_default_lib, (), (override));
  MOCK_METHOD(LibraryContextPtr, create_default_context, (), (override));
  MOCK_METHOD(ILibPtr, create_appropriate_lib, (LibraryContextPtr ctx),
              (override));
  MOCK_METHOD(ExportContextPtr, create_export_context, (), (override));
  MOCK_METHOD(IExporterPtr, create_exporter, (const LibraryContextPtr& ctx),
              (override));
  MOCK_METHOD(IImageCropperFacilityPtr, create_image_cropper, (), (override));

  inline static LibFactoryPtr create_factory()
  {
    return std::make_shared<LibFactory>();
  }
};

using LibFactoryPtr = LibFactory::LibFactoryPtr;

}  // namespace iade0impl

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBFACTORY_CLASS_H
