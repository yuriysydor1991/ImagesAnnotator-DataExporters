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

#include "src/lib/libmain/LibMain.h"

#include <cassert>
#include <memory>

#include "src/lib/libmain/LibFactory.h"
#include "src/log/log.h"

namespace iade0impl
{

LibMain::LibMain()
{
  static bool inited{false};

  if (!inited) {
    LOG_INIT_DEFAULTS();
    inited = true;
  }
}

bool LibMain::libcall(LibraryContextPtr ctx)
{
  assert(ctx != nullptr);

  if (ctx == nullptr) {
    LOGE("No valid library context pointer provided");
    return false;
  }

  auto libFactory = LibFactory::create_factory();

  assert(libFactory != nullptr);

  auto exporter = libFactory->create_exporter(ctx->format);

  if (exporter == nullptr) {
    LOGE("No exporter available for the requested export format");
    return false;
  }

  ctx->exporter = exporter;

  auto ectx = libFactory->create_export_context();

  assert(ectx != nullptr);

  ectx->export_path = ctx->export_path;
  ectx->dbProvider = ctx->dbProvider;
  ectx->cropper = ctx->cropper;

  LOGI("Exporting the annotations database to " << ctx->export_path);

  if (!exporter->export_db(ectx)) {
    LOGE("Fail to export the annotations database to " << ctx->export_path);
    return false;
  }

  return true;
}

LibMainPtr LibMain::create() { return std::make_shared<LibMain>(); }

}  // namespace iade0impl
