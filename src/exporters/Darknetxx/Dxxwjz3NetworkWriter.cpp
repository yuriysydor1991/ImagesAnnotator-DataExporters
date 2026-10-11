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

#include "src/exporters/Darknetxx/Dxxwjz3NetworkWriter.h"

#include <cstddef>
#include <ostream>
#include <string>
#include <vector>

namespace iannotator::exporters
{

Dxxwjz3NetworkWriter::Dxxwjz3NetworkWriter(
    std::ostream& destination, const std::string& cfgText,
    const std::vector<std::string>& classNames)
    : Dxxwjz2NetworkWriter{destination, cfgText, classNames}, cfgJson{cfgText}
{
}

bool Dxxwjz3NetworkWriter::keeps_cfg() const { return cfgJson.valid(); }

const std::string& Dxxwjz3NetworkWriter::format_name() const
{
  return formatName;
}

int Dxxwjz3NetworkWriter::format_version() const { return formatVersion; }

void Dxxwjz3NetworkWriter::network()
{
  stream << ",\n  \"network\": {\"net\": {" << cfgJson.net()
         << "}, \"layers\": [";

  const std::vector<std::string>& layers = cfgJson.layers();

  for (std::size_t index = 0; index < layers.size(); ++index) {
    stream << (index == 0 ? "\n    {" : ",\n    {") << layers[index] << "}";
  }

  stream << (layers.empty() ? "], " : "\n  ], ") << "\"classes\": [";

  for (std::size_t index = 0; index < classes.size(); ++index) {
    stream << (index == 0 ? "" : ", ") << quote(classes[index]);
  }

  stream << "]}";
}

void Dxxwjz3NetworkWriter::training()
{
  // the losses of an index are the ones of the iteration of the index + 1, so
  // the record of no iteration keeps no iteration column either
  stream << ",\n  \"training\": {\"seconds\": 0,\n"
         << "    \"loss\": [],\n"
         << "    \"avg_loss\": []\n"
         << "  }";
}

}  // namespace iannotator::exporters
