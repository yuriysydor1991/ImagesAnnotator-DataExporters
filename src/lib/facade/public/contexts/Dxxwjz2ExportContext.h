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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ2EXPORTCONTEXT_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ2EXPORTCONTEXT_CLASS_H

#include <memory>

#include "../ExportersAPI.h"
#include "../IADataExportersContext.h"

namespace ImagesAnnotatorDataExporters012
{

/**
 * @brief The library context which writes the darknetxx training directory:
 * the YOLO v4 (darknet) one with the network to train beside its cfg file.
 *
 * The darknetxx (https://github.com/yuriysydor1991/darknetxx) keeps a network
 * in the dxxwjz2 file: the gzip compressed JSON document of the network cfg
 * text, the class names at their indexes, the training record and the
 * weights. The `cfg/yolov4-obj.dxxwjz2` file this layout adds is the network
 * alone - of no weights and of no training record - which the darknetxx
 * trains from scratch, while the rest of the directory is the very one the
 * Yolo4ExportContext writes, so the darknetxx-gui trains it as well.
 *
 * The class carries no data of its own: instantiating it is what names the
 * wanted dataset layout, everything else is inherited from
 * IADataExportersContext.
 *
 * Current file is a target for the library header installation.
 */
class IADE_API Dxxwjz2ExportContext : public IADataExportersContext
{
 public:
  using Dxxwjz2ExportContextPtr = std::shared_ptr<Dxxwjz2ExportContext>;
};

using Dxxwjz2ExportContextPtr = Dxxwjz2ExportContext::Dxxwjz2ExportContextPtr;

}  // namespace ImagesAnnotatorDataExporters012

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ2EXPORTCONTEXT_CLASS_H
