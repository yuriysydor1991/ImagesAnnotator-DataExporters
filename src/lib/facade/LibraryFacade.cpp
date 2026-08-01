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

#include "LibraryFacade.h"

#include <cassert>
#include <memory>
#include <string>

#include "project-global-decls.h"
#include "src/lib/libmain/LibFactory.h"

namespace ImagesAnnotatorDataExporters011
{

LibraryContextPtr LibraryFacade::create_library_context()
{
  auto libFactory = lib0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_default_context();
}

ILibPtr LibraryFacade::create_default_lib()
{
  auto libFactory = lib0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_default_lib();
}

ILibPtr LibraryFacade::create_library(LibraryContextPtr ctx)
{
  auto libFactory = lib0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_appropriate_lib(ctx);
}

ExportContextPtr LibraryFacade::create_export_context()
{
  auto libFactory = lib0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_export_context();
}

IExporterPtr LibraryFacade::create_exporter(const ExportFormat& format)
{
  auto libFactory = lib0impl::LibFactory::create_factory();

  assert(libFactory != nullptr);

  return libFactory->create_exporter(format);
}

std::string LibraryFacade::library_version()
{
  return project_decls::PROJECT_BUILD_VERSION;
}

}  // namespace ImagesAnnotatorDataExporters011
