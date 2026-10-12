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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ3PEXPORTCONTEXT_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ3PEXPORTCONTEXT_CLASS_H

#include <memory>

#include "../ExportersAPI.h"
#include "../IADataExportersContext.h"

namespace ImagesAnnotatorDataExporters013
{

/**
 * @brief The library context which writes the darknetxx dxxwjz3p training
 * project: the `yolov4-obj.dxxwjz3p` file beside the `images/` folder of the
 * copies of the annotated images and the empty `backup/` one.
 *
 * The dxxwjz3p file of the darknetxx
 * (https://github.com/yuriysydor1991/darknetxx) is the dxxwjz2p project of the
 * dxxwjz3 document, the whole training project of the JSON data alone: the
 * network cfg is the JSON structure of the options of its sections. The project
 * this layout writes is of no weights, the one the darknetxx trains from
 * scratch.
 *
 * The class carries no data of its own: instantiating it is what names the
 * wanted dataset layout, everything else is inherited from
 * IADataExportersContext.
 *
 * Current file is a target for the library header installation.
 */
class IADE_API Dxxwjz3pExportContext : public IADataExportersContext
{
 public:
  using Dxxwjz3pExportContextPtr = std::shared_ptr<Dxxwjz3pExportContext>;
};

using Dxxwjz3pExportContextPtr =
    Dxxwjz3pExportContext::Dxxwjz3pExportContextPtr;

}  // namespace ImagesAnnotatorDataExporters013

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ3PEXPORTCONTEXT_CLASS_H
