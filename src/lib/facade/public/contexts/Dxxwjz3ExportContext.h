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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ3EXPORTCONTEXT_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ3EXPORTCONTEXT_CLASS_H

#include <memory>

#include "../ExportersAPI.h"
#include "../IADataExportersContext.h"

namespace ImagesAnnotatorDataExporters014
{

/**
 * @brief The library context which writes the darknetxx training directory of
 * the dxxwjz3 network to train: the YOLO v4 (darknet) one with the
 * `cfg/yolov4-obj.dxxwjz3` file beside its cfg file.
 *
 * The dxxwjz3 file of the darknetxx
 * (https://github.com/yuriysydor1991/darknetxx) is the dxxwjz2 one of nothing
 * but the JSON data: the network cfg is the JSON structure of the options of
 * its sections instead of the cfg text. The file this layout adds is the
 * network alone - of no weights and of no training record - which the darknetxx
 * trains from scratch, while the rest of the directory is the very one the
 * Yolo4ExportContext writes.
 *
 * The class carries no data of its own: instantiating it is what names the
 * wanted dataset layout, everything else is inherited from
 * IADataExportersContext.
 *
 * Current file is a target for the library header installation.
 */
class IADE_API Dxxwjz3ExportContext : public IADataExportersContext
{
 public:
  using Dxxwjz3ExportContextPtr = std::shared_ptr<Dxxwjz3ExportContext>;
};

using Dxxwjz3ExportContextPtr = Dxxwjz3ExportContext::Dxxwjz3ExportContextPtr;

}  // namespace ImagesAnnotatorDataExporters014

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ3EXPORTCONTEXT_CLASS_H
