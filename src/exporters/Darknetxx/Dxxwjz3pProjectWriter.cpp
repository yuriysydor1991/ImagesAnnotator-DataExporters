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

#include "src/exporters/Darknetxx/Dxxwjz3pProjectWriter.h"

#include <ostream>
#include <string>
#include <vector>

namespace iannotator::exporters
{

Dxxwjz3pProjectWriter::Dxxwjz3pProjectWriter(
    std::ostream& destination, const std::string& cfgText,
    const std::vector<std::string>& classNames, const std::string& folderPath,
    const Dxxwjz2pAnnotationsWriter::Images& folderImages)
    : Dxxwjz3NetworkWriter{destination, cfgText, classNames},
      folder{folderPath},
      images{folderImages}
{
}

const std::string& Dxxwjz3pProjectWriter::format_name() const
{
  return formatName;
}

int Dxxwjz3pProjectWriter::format_version() const { return formatVersion; }

void Dxxwjz3pProjectWriter::members()
{
  Dxxwjz2pAnnotationsWriter{stream, folder, images}.write();
}

}  // namespace iannotator::exporters
