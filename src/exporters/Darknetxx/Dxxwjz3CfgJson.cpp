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

#include "src/exporters/Darknetxx/Dxxwjz3CfgJson.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <limits>
#include <locale>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "src/exporters/Darknetxx/Dxxwjz2NetworkWriter.h"

namespace iannotator::exporters
{

namespace
{

/// @brief The strip() of the darknet: drops every whitespace of the line.
std::string strip(const std::string& line)
{
  std::string stripped;
  stripped.reserve(line.size());

  for (const char symbol : line) {
    if (symbol != ' ' && symbol != '\t' && symbol != '\n' && symbol != '\r') {
      stripped.push_back(symbol);
    }
  }

  return stripped;
}

/// @brief Tells the text is the integer the std::from_chars() reads whole: the
/// digits of an optional minus sign, of the 64 bits range.
bool read_integer(const std::string& text, int64_t& value)
{
  const std::size_t digits = !text.empty() && text[0] == '-' ? 1U : 0U;

  if (text.size() == digits ||
      text.find_first_not_of("0123456789", digits) != std::string::npos) {
    return false;
  }

  std::istringstream stream{text};
  stream.imbue(std::locale::classic());

  return static_cast<bool>(stream >> value) && stream.peek() == EOF;
}

}  // namespace

Dxxwjz3CfgJson::Dxxwjz3CfgJson(const std::string& cfgText)
{
  std::vector<Section> sections;

  if (!read_sections(cfgText, sections) || sections.empty() ||
      (sections.front().header != "[net]" &&
       sections.front().header != "[network]")) {
    return;
  }

  for (std::size_t index = 1U; index < sections.size(); ++index) {
    const std::string& header = sections[index].header;

    if (header.size() < 3U || header.back() != ']') {
      layerMembers.clear();
      return;
    }

    const std::string inner = members(sections[index].options);

    layerMembers.push_back(
        key(typeKey) +
        Dxxwjz2NetworkWriter::quote(header.substr(1U, header.size() - 2U)) +
        (inner.empty() ? "" : ", " + key(optionsKey) + "{" + inner + "}"));
  }

  netMembers = members(sections.front().options);
  isValid = true;
}

bool Dxxwjz3CfgJson::valid() const { return isValid; }

const std::string& Dxxwjz3CfgJson::net() const { return netMembers; }

const std::vector<std::string>& Dxxwjz3CfgJson::layers() const
{
  return layerMembers;
}

std::string Dxxwjz3CfgJson::json_of(const std::optional<std::string>& value)
{
  if (!value.has_value()) {
    return "null";
  }

  const std::string& text = *value;
  std::string items;
  std::size_t start = 0U;

  for (std::size_t comma = text.find(','); comma != std::string::npos;
       start = comma + 1U, comma = text.find(',', start)) {
    items += item_json(text.substr(start, comma - start)) + ",";
  }

  items += item_json(text.substr(start));

  return start == 0U ? items : "[" + items + "]";
}

std::string Dxxwjz3CfgJson::fraction_text(const float value)
{
  std::ostringstream text;
  text.imbue(std::locale::classic());

  // the fewest decimals the float is read back of: the 45 of the least
  // denormal one at the most
  for (int decimals = 0; decimals <= 50; ++decimals) {
    text.str({});
    text << std::fixed << std::setprecision(decimals)
         << static_cast<double>(value);

    float back = 0.f;

    if (read_float(text.str(), back) && back == value) {
      break;
    }
  }

  return text.str();
}

bool Dxxwjz3CfgJson::read_sections(const std::string& cfgText,
                                   std::vector<Section>& sections)
{
  std::istringstream text{cfgText};
  std::string raw;

  while (std::getline(text, raw)) {
    const std::string line = strip(raw);

    if (line.empty() || line[0] == '#' || line[0] == ';') {
      continue;
    }

    if (line[0] == '[') {
      sections.push_back(Section{line, {}});
      continue;
    }

    if (sections.empty()) {
      return false;
    }

    const std::size_t separator = line.find('=');

    if (separator == std::string::npos) {
      sections.back().options.emplace_back(line, std::nullopt);
    } else if (separator + 1U < line.size()) {
      sections.back().options.emplace_back(line.substr(0, separator),
                                           line.substr(separator + 1U));
    }
  }

  return true;
}

std::string Dxxwjz3CfgJson::members(const Options& options)
{
  std::set<std::string> keys;
  std::string json;

  for (const auto& [name, value] : options) {
    if (keys.insert(name).second) {
      json += (json.empty() ? "" : ", ") + key(name) + json_of(value);
    }
  }

  return json;
}

std::string Dxxwjz3CfgJson::item_json(const std::string& item)
{
  // the std::from_chars() reads the numbers of no plus sign
  const std::string digits =
      item.size() > 1U && item[0] == '+' ? item.substr(1U) : item;
  int64_t whole = 0;
  float fraction = 0.f;
  std::string number;

  if (read_integer(digits, whole)) {
    number = std::to_string(whole);
  } else if (read_float(digits, fraction) && std::isfinite(fraction)) {
    number = fraction_text(fraction);
  }

  // the atof() of the number is the float of the item, the atoi() of the
  // integer part is the one to check
  const bool same =
      !number.empty() && std::strtoll(number.c_str(), nullptr, 10) ==
                             std::strtoll(item.c_str(), nullptr, 10);

  return same ? number : Dxxwjz2NetworkWriter::quote(item);
}

bool Dxxwjz3CfgJson::read_float(const std::string& text, float& value)
{
  // the decimal of the std::from_chars(): the digits of an optional minus
  // sign and of an optional point, at least one of them, and the exponent
  const std::size_t sign = !text.empty() && text[0] == '-' ? 1U : 0U;
  const std::size_t exponent = text.find_first_of("eE", sign);
  const std::string mantissa = text.substr(sign, exponent - sign);
  const std::size_t point = mantissa.find('.');

  if (mantissa.find_first_not_of("0123456789.") != std::string::npos ||
      mantissa.find_first_of("0123456789") == std::string::npos ||
      (point != std::string::npos &&
       mantissa.find('.', point + 1U) != std::string::npos)) {
    return false;
  }

  if (exponent != std::string::npos) {
    const std::size_t power =
        exponent + 1U < text.size() &&
                (text[exponent + 1U] == '+' || text[exponent + 1U] == '-')
            ? exponent + 2U
            : exponent + 1U;

    if (power == text.size() ||
        text.find_first_not_of("0123456789", power) != std::string::npos) {
      return false;
    }
  }

  std::istringstream stream{text};
  stream.imbue(std::locale::classic());

  float parsed = 0.f;

  if (!(stream >> parsed) || stream.peek() != EOF ||
      std::abs(parsed) > std::numeric_limits<float>::max()) {
    return false;
  }

  value = parsed;

  return true;
}

std::string Dxxwjz3CfgJson::key(const std::string& name)
{
  return Dxxwjz2NetworkWriter::quote(name) + ": ";
}

}  // namespace iannotator::exporters
