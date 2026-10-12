#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBRARYMAIN_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBRARYMAIN_CLASS_H

#include <gmock/gmock.h>

#include <memory>

#include "IADataExportersContext.h"
#include "IADataExportersLib.h"

namespace iade0impl
{

class LibMain : public ImagesAnnotatorDataExporters014::IADataExportersLib
{
 public:
  using LibMainPtr = std::shared_ptr<LibMain>;
  using IADataExportersContextPtr =
      ImagesAnnotatorDataExporters014::IADataExportersContextPtr;

  virtual ~LibMain() = default;
  LibMain() = default;

  MOCK_METHOD(bool, perform_export, (IADataExportersContextPtr ctx),
              (override));

  inline static LibMainPtr create() { return std::make_shared<LibMain>(); }
};

using LibMainPtr = LibMain::LibMainPtr;

}  // namespace iade0impl

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_LIBRARYMAIN_CLASS_H
