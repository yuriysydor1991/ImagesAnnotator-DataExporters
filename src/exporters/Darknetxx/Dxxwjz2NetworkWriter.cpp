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

#include "src/exporters/Darknetxx/Dxxwjz2NetworkWriter.h"

#include <cstddef>
#include <ostream>
#include <string>
#include <vector>

#include "project-global-decls.h"

namespace iannotator::exporters
{

Dxxwjz2NetworkWriter::Dxxwjz2NetworkWriter(
    std::ostream& destination, const std::string& cfgText,
    const std::vector<std::string>& classNames)
    : stream{destination}, cfg{cfgText}, classes{classNames}
{
}

bool Dxxwjz2NetworkWriter::write()
{
  if (!keeps_cfg()) {
    return false;
  }

  stream << "{\n"
         << "  \"format\": " << quote(format_name()) << ",\n"
         << "  \"version\": " << std::to_string(format_version()) << ",\n"
         << "  \"generator\": " << quote(generator());

  network();
  training();
  members();

  stream << "\n}\n";

  return true;
}

std::string Dxxwjz2NetworkWriter::generator()
{
  return project_decls::PROJECT_NAME + " " +
         project_decls::PROJECT_BUILD_VERSION;
}

bool Dxxwjz2NetworkWriter::keeps_cfg() const { return !cfg.empty(); }

const std::string& Dxxwjz2NetworkWriter::format_name() const
{
  return formatName;
}

int Dxxwjz2NetworkWriter::format_version() const { return formatVersion; }

void Dxxwjz2NetworkWriter::network()
{
  stream << ",\n  \"network\": {\"cfg\": " << quote(cfg) << ", \"classes\": [";

  for (std::size_t index = 0; index < classes.size(); ++index) {
    stream << (index == 0 ? "\n    {" : ",\n    {")
           << "\"index\": " << std::to_string(index)
           << ", \"name\": " << quote(classes[index]) << "}";
  }

  stream << (classes.empty() ? "]}" : "\n  ]}");
}

void Dxxwjz2NetworkWriter::training()
{
  // the network to train has trained for no iteration yet: the record is
  // there, the way the darknetxx writes it, but every column of it is empty
  stream << ",\n  \"training\": {\"seconds\": 0,\n"
         << "    \"iteration\": [],\n"
         << "    \"loss\": [],\n"
         << "    \"avg_loss\": []\n"
         << "  }";
}

void Dxxwjz2NetworkWriter::members() {}

std::string Dxxwjz2NetworkWriter::quote(const std::string& text)
{
  std::string quoted{"\""};

  for (std::size_t position = 0; position < text.size();) {
    const std::size_t length = utf8_sequence(text, position);

    if (length == 0) {
      quoted += "\xEF\xBF\xBD";  // the U+FFFD replacement character
      ++position;
    } else if (length == 1) {
      escape(text[position++], quoted);
    } else {
      quoted.append(text, position, length);
      position += length;
    }
  }

  return quoted + "\"";
}

std::size_t Dxxwjz2NetworkWriter::utf8_sequence(const std::string& text,
                                                std::size_t position)
{
  // a byte past the end of the text fits no range below
  const auto byte = [&text, position](std::size_t offset) {
    return position + offset < text.size()
               ? static_cast<unsigned>(
                     static_cast<unsigned char>(text[position + offset]))
               : 0x100U;
  };

  const unsigned first = byte(0);

  if (first < 0x80U) {
    return 1;
  }

  // the length of the sequence and the range of its second byte, which keeps
  // out the overlong forms, the surrogates and what lies beyond U+10FFFF
  std::size_t length{0};
  unsigned low{0x80U};
  unsigned high{0xBFU};

  if (first >= 0xC2U && first <= 0xDFU) {
    length = 2;
  } else if (first >= 0xE0U && first <= 0xEFU) {
    length = 3;
    low = first == 0xE0U ? 0xA0U : low;
    high = first == 0xEDU ? 0x9FU : high;
  } else if (first >= 0xF0U && first <= 0xF4U) {
    length = 4;
    low = first == 0xF0U ? 0x90U : low;
    high = first == 0xF4U ? 0x8FU : high;
  } else {
    return 0;
  }

  if (byte(1) < low || byte(1) > high) {
    return 0;
  }

  for (std::size_t offset = 2; offset < length; ++offset) {
    if (byte(offset) < 0x80U || byte(offset) > 0xBFU) {
      return 0;
    }
  }

  return length;
}

void Dxxwjz2NetworkWriter::escape(char symbol, std::string& quoted)
{
  static const std::string hexDigits{"0123456789abcdef"};

  const auto code = static_cast<unsigned char>(symbol);

  if (symbol == '"' || symbol == '\\') {
    quoted.push_back('\\');
    quoted.push_back(symbol);
  } else if (symbol == '\n') {
    quoted += "\\n";
  } else if (symbol == '\r') {
    quoted += "\\r";
  } else if (symbol == '\t') {
    quoted += "\\t";
  } else if (code < 0x20U) {
    quoted += "\\u00";
    quoted.push_back(hexDigits[(code >> 4U) & 0xFU]);
    quoted.push_back(hexDigits[code & 0xFU]);
  } else {
    quoted.push_back(symbol);
  }
}

}  // namespace iannotator::exporters
