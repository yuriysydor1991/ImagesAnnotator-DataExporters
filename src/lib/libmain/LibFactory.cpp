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

#include "src/lib/libmain/LibFactory.h"

#include <cassert>
#include <memory>

#include "CocoExportLibraryContext.h"
#include "CreateMLExportLibraryContext.h"
#include "PascalVocExportLibraryContext.h"
#include "PlainTxtExportLibraryContext.h"
#include "PyTorchExportLibraryContext.h"
#include "UltralyticsDetectExportLibraryContext.h"
#include "UltralyticsObbExportLibraryContext.h"
#include "UltralyticsSegmentExportLibraryContext.h"
#include "Yolo4ExportLibraryContext.h"
#include "src/croppers/ImageCropperFactory.h"
#include "src/exporters/Coco/Coco2FolderExporter.h"
#include "src/exporters/CreateML/CreateML2FolderExporter.h"
#include "src/exporters/PascalVoc/PascalVoc2FolderExporter.h"
#include "src/exporters/PlainTxt/PlainTxt2FolderExporter.h"
#include "src/exporters/PyTorch/PyTorchVisionFolderExporter.h"
#include "src/exporters/Ultralytics/UltralyticsDetect2FolderExporter.h"
#include "src/exporters/Ultralytics/UltralyticsObb2FolderExporter.h"
#include "src/exporters/Ultralytics/UltralyticsSegment2FolderExporter.h"
#include "src/exporters/Yolo4/Yolo42FolderExporter.h"
#include "src/lib/libmain/LibMain.h"
#include "src/log/log.h"

namespace iade0impl
{

using namespace ImagesAnnotatorDataExporters011;

LibFactory::ILibPtr LibFactory::create_default_lib()
{
  return LibMain::create();
}

LibFactory::PlainTxtExportLibraryContextPtr
LibFactory::create_plain_txt_library_context()
{
  return std::make_shared<PlainTxtExportLibraryContext>();
}

LibFactory::Yolo4ExportLibraryContextPtr
LibFactory::create_yolo4_library_context()
{
  return std::make_shared<Yolo4ExportLibraryContext>();
}

LibFactory::UltralyticsDetectExportLibraryContextPtr
LibFactory::create_ultralytics_detect_library_context()
{
  return std::make_shared<UltralyticsDetectExportLibraryContext>();
}

LibFactory::UltralyticsObbExportLibraryContextPtr
LibFactory::create_ultralytics_obb_library_context()
{
  return std::make_shared<UltralyticsObbExportLibraryContext>();
}

LibFactory::UltralyticsSegmentExportLibraryContextPtr
LibFactory::create_ultralytics_segment_library_context()
{
  return std::make_shared<UltralyticsSegmentExportLibraryContext>();
}

LibFactory::CocoExportLibraryContextPtr
LibFactory::create_coco_library_context()
{
  return std::make_shared<CocoExportLibraryContext>();
}

LibFactory::PascalVocExportLibraryContextPtr
LibFactory::create_pascal_voc_library_context()
{
  return std::make_shared<PascalVocExportLibraryContext>();
}

LibFactory::CreateMLExportLibraryContextPtr
LibFactory::create_createml_library_context()
{
  return std::make_shared<CreateMLExportLibraryContext>();
}

LibFactory::PyTorchExportLibraryContextPtr
LibFactory::create_pytorch_library_context()
{
  return std::make_shared<PyTorchExportLibraryContext>();
}

LibFactory::ILibPtr LibFactory::create_appropriate_lib(
    [[maybe_unused]] LibraryContextPtr ctx)
{
  assert(ctx != nullptr);

  return create_default_lib();
}

// The wanted layout is the very type of the context descendant, whatever data
// that descendant carries of its own. A cast per layout keeps that knowledge
// here, where the concrete exporter classes are already known, instead of in
// the installed headers.
LibFactory::IExporterPtr LibFactory::create_exporter(
    const LibraryContextPtr& ctx)
{
  if (std::dynamic_pointer_cast<PlainTxtExportLibraryContext>(ctx) != nullptr) {
    return std::make_shared<iannotator::exporters::PlainTxt2FolderExporter>();
  }

  if (std::dynamic_pointer_cast<Yolo4ExportLibraryContext>(ctx) != nullptr) {
    return std::make_shared<iannotator::exporters::Yolo42FolderExporter>();
  }

  if (std::dynamic_pointer_cast<UltralyticsDetectExportLibraryContext>(ctx) !=
      nullptr) {
    return std::make_shared<
        iannotator::exporters::UltralyticsDetect2FolderExporter>();
  }

  if (std::dynamic_pointer_cast<UltralyticsObbExportLibraryContext>(ctx) !=
      nullptr) {
    return std::make_shared<
        iannotator::exporters::UltralyticsObb2FolderExporter>();
  }

  if (std::dynamic_pointer_cast<UltralyticsSegmentExportLibraryContext>(ctx) !=
      nullptr) {
    return std::make_shared<
        iannotator::exporters::UltralyticsSegment2FolderExporter>();
  }

  if (std::dynamic_pointer_cast<CocoExportLibraryContext>(ctx) != nullptr) {
    return std::make_shared<iannotator::exporters::Coco2FolderExporter>();
  }

  if (std::dynamic_pointer_cast<PascalVocExportLibraryContext>(ctx) !=
      nullptr) {
    return std::make_shared<iannotator::exporters::PascalVoc2FolderExporter>();
  }

  if (std::dynamic_pointer_cast<CreateMLExportLibraryContext>(ctx) != nullptr) {
    return std::make_shared<iannotator::exporters::CreateML2FolderExporter>();
  }

  if (std::dynamic_pointer_cast<PyTorchExportLibraryContext>(ctx) != nullptr) {
    return std::make_shared<
        iannotator::exporters::PyTorchVisionFolderExporter>();
  }

  LOGE("No library context of a known dataset layout given");

  return nullptr;
}

LibFactory::IImageCropperFacilityPtr LibFactory::create_image_cropper()
{
  return iannotator::exporters::croppers::create_builtin_cropper();
}

LibFactoryPtr LibFactory::create_factory()
{
  return std::make_shared<LibFactory>();
}

}  // namespace iade0impl
