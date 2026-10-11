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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ3PPROJECTEXPORTER_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ3PPROJECTEXPORTER_CLASS_H

#include <ostream>
#include <string>
#include <vector>

#include "src/exporters/Darknetxx/Dxxwjz2pProjectExporter.h"

namespace iannotator::exporters
{

/**
 * @brief The darknetxx dxxwjz3p training project exporter.
 *
 * The directory is the one the Dxxwjz2pProjectExporter writes, but the
 * project is the dxxwjz3p document of the darknetxx
 * (https://github.com/yuriysydor1991/darknetxx), the whole training project of
 * the JSON data alone: the network of the JSON options of the cfg sections
 * instead of the cfg text (see the Dxxwjz3pProjectWriter).
 *
 * @code
 * <export directory>/
 *   yolov4-obj.dxxwjz3p     the project, of the image paths relative to it
 *   images/                 the copies of the annotated images
 *   backup/                 the weights files the darknetxx training saves
 * @endcode
 *
 * The darknetxx trains it from scratch, from inside of the exported directory
 * (the darknetxx-train-gui opens it as well):
 *
 * @code
 * darknetxx --train-cfg yolov4-obj.dxxwjz3p
 * @endcode
 */
class Dxxwjz3pProjectExporter : public Dxxwjz2pProjectExporter
{
 public:
  virtual ~Dxxwjz3pProjectExporter() = default;
  Dxxwjz3pProjectExporter() = default;

 protected:
  virtual const std::string& project_extension() const override;
  virtual bool write_project(std::ostream& document, const std::string& cfgText,
                             const std::vector<std::string>& classes,
                             const Images& images) const override;
};

}  // namespace iannotator::exporters

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ3PPROJECTEXPORTER_CLASS_H
