#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBRARYMAIN_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBRARYMAIN_CLASS_H

#include <gmock/gmock.h>

#include <memory>

#include "ILib.h"
#include "LibraryContext.h"

namespace iade0impl
{

class LibMain : public ImagesAnnotatorDataExporters011::ILib
{
 public:
  using LibMainPtr = std::shared_ptr<LibMain>;
  using LibraryContextPtr = ImagesAnnotatorDataExporters011::LibraryContextPtr;

  virtual ~LibMain() = default;
  LibMain() = default;

  MOCK_METHOD(bool, libcall, (LibraryContextPtr ctx), (override));

  inline static LibMainPtr create() { return std::make_shared<LibMain>(); }
};

using LibMainPtr = LibMain::LibMainPtr;

}  // namespace iade0impl

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBRARYMAIN_CLASS_H
