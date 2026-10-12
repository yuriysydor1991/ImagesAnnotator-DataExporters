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
#include "src/croppers/ImageCropperFactory.h"
#include "src/exporters/Coco/Coco2FolderExporter.h"
#include "src/exporters/CreateML/CreateML2FolderExporter.h"
#include "src/exporters/Darknetxx/Dxxwjz2FolderExporter.h"
#include "src/exporters/Darknetxx/Dxxwjz2pProjectExporter.h"
#include "src/exporters/Darknetxx/Dxxwjz3FolderExporter.h"
#include "src/exporters/Darknetxx/Dxxwjz3pProjectExporter.h"
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

using namespace ImagesAnnotatorDataExporters013;

LibFactory::IADataExportersLibPtr LibFactory::create_default_lib()
{
  return LibMain::create();
}

LibFactory::PlainTxtExportContextPtr
LibFactory::create_plain_txt_library_context()
{
  return std::make_shared<PlainTxtExportContext>();
}

LibFactory::Yolo4ExportContextPtr LibFactory::create_yolo4_library_context()
{
  return std::make_shared<Yolo4ExportContext>();
}

LibFactory::UltralyticsDetectExportContextPtr
LibFactory::create_ultralytics_detect_library_context()
{
  return std::make_shared<UltralyticsDetectExportContext>();
}

LibFactory::UltralyticsObbExportContextPtr
LibFactory::create_ultralytics_obb_library_context()
{
  return std::make_shared<UltralyticsObbExportContext>();
}

LibFactory::UltralyticsSegmentExportContextPtr
LibFactory::create_ultralytics_segment_library_context()
{
  return std::make_shared<UltralyticsSegmentExportContext>();
}

LibFactory::CocoExportContextPtr LibFactory::create_coco_library_context()
{
  return std::make_shared<CocoExportContext>();
}

LibFactory::PascalVocExportContextPtr
LibFactory::create_pascal_voc_library_context()
{
  return std::make_shared<PascalVocExportContext>();
}

LibFactory::CreateMLExportContextPtr
LibFactory::create_createml_library_context()
{
  return std::make_shared<CreateMLExportContext>();
}

LibFactory::Dxxwjz2ExportContextPtr LibFactory::create_dxxwjz2_library_context()
{
  return std::make_shared<Dxxwjz2ExportContext>();
}

LibFactory::Dxxwjz3ExportContextPtr LibFactory::create_dxxwjz3_library_context()
{
  return std::make_shared<Dxxwjz3ExportContext>();
}

LibFactory::Dxxwjz2pExportContextPtr
LibFactory::create_dxxwjz2p_library_context()
{
  return std::make_shared<Dxxwjz2pExportContext>();
}

LibFactory::Dxxwjz3pExportContextPtr
LibFactory::create_dxxwjz3p_library_context()
{
  return std::make_shared<Dxxwjz3pExportContext>();
}

LibFactory::PyTorchExportContextPtr LibFactory::create_pytorch_library_context()
{
  return std::make_shared<PyTorchExportContext>();
}

LibFactory::IADataExportersLibPtr LibFactory::create_appropriate_lib(
    [[maybe_unused]] IADataExportersContextPtr ctx)
{
  assert(ctx != nullptr);

  return create_default_lib();
}

// The wanted layout is the very type of the context descendant, whatever data
// that descendant carries of its own. A cast per layout keeps that knowledge
// here, where the concrete exporter classes are already known, instead of in
// the installed headers.
LibFactory::IExporterPtr LibFactory::create_exporter(
    const IADataExportersContextPtr& ctx)
{
  if (std::dynamic_pointer_cast<PlainTxtExportContext>(ctx) != nullptr) {
    return std::make_shared<iannotator::exporters::PlainTxt2FolderExporter>();
  }

  if (std::dynamic_pointer_cast<Yolo4ExportContext>(ctx) != nullptr) {
    return std::make_shared<iannotator::exporters::Yolo42FolderExporter>();
  }

  if (std::dynamic_pointer_cast<UltralyticsDetectExportContext>(ctx) !=
      nullptr) {
    return std::make_shared<
        iannotator::exporters::UltralyticsDetect2FolderExporter>();
  }

  if (std::dynamic_pointer_cast<UltralyticsObbExportContext>(ctx) != nullptr) {
    return std::make_shared<
        iannotator::exporters::UltralyticsObb2FolderExporter>();
  }

  if (std::dynamic_pointer_cast<UltralyticsSegmentExportContext>(ctx) !=
      nullptr) {
    return std::make_shared<
        iannotator::exporters::UltralyticsSegment2FolderExporter>();
  }

  if (std::dynamic_pointer_cast<CocoExportContext>(ctx) != nullptr) {
    return std::make_shared<iannotator::exporters::Coco2FolderExporter>();
  }

  if (std::dynamic_pointer_cast<PascalVocExportContext>(ctx) != nullptr) {
    return std::make_shared<iannotator::exporters::PascalVoc2FolderExporter>();
  }

  if (std::dynamic_pointer_cast<CreateMLExportContext>(ctx) != nullptr) {
    return std::make_shared<iannotator::exporters::CreateML2FolderExporter>();
  }

  if (std::dynamic_pointer_cast<Dxxwjz2ExportContext>(ctx) != nullptr) {
    return std::make_shared<iannotator::exporters::Dxxwjz2FolderExporter>();
  }

  if (std::dynamic_pointer_cast<Dxxwjz3ExportContext>(ctx) != nullptr) {
    return std::make_shared<iannotator::exporters::Dxxwjz3FolderExporter>();
  }

  if (std::dynamic_pointer_cast<Dxxwjz2pExportContext>(ctx) != nullptr) {
    return std::make_shared<iannotator::exporters::Dxxwjz2pProjectExporter>();
  }

  if (std::dynamic_pointer_cast<Dxxwjz3pExportContext>(ctx) != nullptr) {
    return std::make_shared<iannotator::exporters::Dxxwjz3pProjectExporter>();
  }

  if (std::dynamic_pointer_cast<PyTorchExportContext>(ctx) != nullptr) {
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
