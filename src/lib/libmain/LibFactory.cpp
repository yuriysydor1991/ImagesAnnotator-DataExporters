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

#include "src/lib/libmain/LibMain.h"
#include "src/lib/libmain/croppers/ImageCropperFactory.h"
#include "src/lib/libmain/exporters/PlainTxt2FolderExporter.h"
#include "src/lib/libmain/exporters/PyTorchVisionFolderExporter.h"
#include "src/lib/libmain/exporters/Yolo42FolderExporter.h"
#include "src/log/log.h"

namespace iade0impl
{

LibFactory::ILibPtr LibFactory::create_default_lib()
{
  return LibMain::create();
}

LibFactory::LibraryContextPtr LibFactory::create_default_context()
{
  return std::make_shared<LibraryContext>();
}

LibFactory::ILibPtr LibFactory::create_appropriate_lib(
    [[maybe_unused]] LibraryContextPtr ctx)
{
  assert(ctx != nullptr);

  return create_default_lib();
}

LibFactory::ExportContextPtr LibFactory::create_export_context()
{
  return std::make_shared<ExportContext>();
}

LibFactory::IExporterPtr LibFactory::create_exporter(const ExportFormat& format)
{
  switch (format) {
    case ExportFormat::PlainTxt2Folder:
      return std::make_shared<iannotator::exporters::PlainTxt2FolderExporter>();
    case ExportFormat::Yolo42Folder:
      return std::make_shared<iannotator::exporters::Yolo42FolderExporter>();
    case ExportFormat::PyTorchVisionFolder:
      return std::make_shared<
          iannotator::exporters::PyTorchVisionFolderExporter>();
  }

  LOGE("Unknown export format requested");

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
