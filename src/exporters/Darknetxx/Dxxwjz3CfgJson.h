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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ3CFGJSON_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ3CFGJSON_CLASS_H

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace iannotator::exporters
{

/**
 * @brief The JSON text of the darknet network cfg text, the way the dxxwjz3
 * files of the darknetxx (https://github.com/yuriysydor1991/darknetxx) keep
 * it: the options of the `[net]` section and the layer objects of the rest of
 * the sections, of the `type` of the section and the `options` of it.
 *
 * The cfg text is read the way the darknet reads it (its read_cfg()): every
 * whitespace of a line is dropped, the empty lines and the ones of the leading
 * `#` or `;` are skipped, a line of the leading `[` begins the next section,
 * the rest are the `key=value` options of the current one, the line of no `=`
 * is the key of the NULL value and the one of nothing after the `=` is
 * dropped. The first option of a key is the one the darknet reads, so the
 * repeated keys are left out.
 *
 * Every value is the JSON value the darknet reads the same as the text: the
 * integer of the integer text, the shortest fixed notation decimal of the
 * float of the fraction text (the `.1` is the `0.1`), the array of the comma
 * separated values, the `null` of the NULL value and the string of the rest
 * (the `1e-3`, the `atoi()` of which is not the one of the `0.001`, stays the
 * `"1e-3"`). So the darknetxx turns the JSON back into the cfg text of the
 * very network:
 *
 * @code
 * [net]                     "net": {"batch": 64, "hue": 0.1}
 * batch=64
 * hue=.1              ->    {"type": "convolutional", "options": {"filters":
 * 32, "activation": "mish"}} [convolutional]           {"type": "route",
 * "options": {"layers": [-1,-7]}} filters=32 activation=mish
 *
 * [route]
 * layers=-1, -7
 * @endcode
 */
class Dxxwjz3CfgJson
{
 public:
  /// @brief The value of the "type" key of a layer object.
  inline static const std::string typeKey{"type"};
  /// @brief The key of the options object of a layer object.
  inline static const std::string optionsKey{"options"};

  explicit Dxxwjz3CfgJson(const std::string& cfgText);

  /**
   * @brief Tells the JSON keeps the cfg text: the first section of the text is
   * the `[net]` (or the `[network]`) one, no option comes before it and every
   * section header is closed by the `]`. The darknetxx refuses the rest.
   */
  bool valid() const;

  /// @brief The members of the `net` object: the options of the `[net]`
  /// section, of no braces.
  const std::string& net() const;

  /// @brief The members of the layer objects in the order of their sections,
  /// of no braces.
  const std::vector<std::string>& layers() const;

  /**
   * @brief The JSON text of the darknet option value, of the NULL one for
   * none: the integer, the decimal, the string or the array of them.
   */
  static std::string json_of(const std::optional<std::string>& value);

  /// @brief The shortest fixed notation decimal text of the float, the one the
  /// float is read back of (the `0.1` of the `.1`, the `1` of the `1.0`).
  static std::string fraction_text(float value);

 private:
  /// @brief The options of a section in their order, of no value for the
  /// line of no `=`.
  using Options =
      std::vector<std::pair<std::string, std::optional<std::string>>>;

  struct Section
  {
    std::string header;
    Options options;
  };

  /// @brief The read_cfg() of the darknet, false for the option before the
  /// first section.
  static bool read_sections(const std::string& cfgText,
                            std::vector<Section>& sections);

  /// @brief The members of the options of the section, the first one of a
  /// repeated key alone.
  static std::string members(const Options& options);

  /// @brief The JSON value of a single item of the comma separated value.
  static std::string item_json(const std::string& item);

  /// @brief The float of the whole text in the C locale, the way the
  /// std::from_chars() reads it (no leading sign plus, no whitespace).
  static bool read_float(const std::string& text, float& value);

  static std::string key(const std::string& name);

  bool isValid{false};
  std::string netMembers;
  std::vector<std::string> layerMembers;
};

}  // namespace iannotator::exporters

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ3CFGJSON_CLASS_H
