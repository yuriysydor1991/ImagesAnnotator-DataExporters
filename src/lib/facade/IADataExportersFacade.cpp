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

#include "IADataExportersFacade.h"

#include <cassert>
#include <memory>
#include <string>

#include "project-global-decls.h"
#include "src/lib/libmain/LibFactory.h"
#include "src/log/log.h"

namespace ImagesAnnotatorDataExporters014
{

PlainTxtExportContextPtr
IADataExportersFacade::create_plain_txt_library_context()
{
  auto libFactory = iade0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_plain_txt_library_context();
}

Yolo4ExportContextPtr IADataExportersFacade::create_yolo4_library_context()
{
  auto libFactory = iade0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_yolo4_library_context();
}

UltralyticsDetectExportContextPtr
IADataExportersFacade::create_ultralytics_detect_library_context()
{
  auto libFactory = iade0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_ultralytics_detect_library_context();
}

UltralyticsObbExportContextPtr
IADataExportersFacade::create_ultralytics_obb_library_context()
{
  auto libFactory = iade0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_ultralytics_obb_library_context();
}

UltralyticsSegmentExportContextPtr
IADataExportersFacade::create_ultralytics_segment_library_context()
{
  auto libFactory = iade0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_ultralytics_segment_library_context();
}

CocoExportContextPtr IADataExportersFacade::create_coco_library_context()
{
  auto libFactory = iade0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_coco_library_context();
}

PascalVocExportContextPtr
IADataExportersFacade::create_pascal_voc_library_context()
{
  auto libFactory = iade0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_pascal_voc_library_context();
}

CreateMLExportContextPtr
IADataExportersFacade::create_createml_library_context()
{
  auto libFactory = iade0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_createml_library_context();
}

Dxxwjz2ExportContextPtr IADataExportersFacade::create_dxxwjz2_library_context()
{
  auto libFactory = iade0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_dxxwjz2_library_context();
}

Dxxwjz3ExportContextPtr IADataExportersFacade::create_dxxwjz3_library_context()
{
  auto libFactory = iade0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_dxxwjz3_library_context();
}

Dxxwjz2pExportContextPtr
IADataExportersFacade::create_dxxwjz2p_library_context()
{
  auto libFactory = iade0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_dxxwjz2p_library_context();
}

Dxxwjz3pExportContextPtr
IADataExportersFacade::create_dxxwjz3p_library_context()
{
  auto libFactory = iade0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_dxxwjz3p_library_context();
}

PyTorchExportContextPtr IADataExportersFacade::create_pytorch_library_context()
{
  auto libFactory = iade0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_pytorch_library_context();
}

IADataExportersLibPtr IADataExportersFacade::create_default_lib()
{
  auto libFactory = iade0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_default_lib();
}

IADataExportersLibPtr IADataExportersFacade::create_library(
    IADataExportersContextPtr ctx)
{
  auto libFactory = iade0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_appropriate_lib(ctx);
}

IExporterPtr IADataExportersFacade::create_exporter(
    const IADataExportersContextPtr& ctx)
{
  auto libFactory = iade0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_exporter(ctx);
}

IImageCropperFacilityPtr IADataExportersFacade::create_image_cropper()
{
  auto libFactory = iade0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_image_cropper();
}

std::string IADataExportersFacade::library_version()
{
  return project_decls::PROJECT_BUILD_VERSION;
}

void IADataExportersFacade::accept_real_logger(
    const std::shared_ptr<logger::ILogger>& realLogger)
{
  LOG_INIT_REAL_LOGGER(realLogger);
}

}  // namespace ImagesAnnotatorDataExporters014
