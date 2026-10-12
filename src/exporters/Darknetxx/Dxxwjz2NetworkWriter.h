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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ2NETWORKWRITER_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ2NETWORKWRITER_CLASS_H

#include <cstddef>
#include <ostream>
#include <string>
#include <vector>

namespace iannotator::exporters
{

/**
 * @brief The JSON document writer of the darknetxx network to train.
 *
 * The darknetxx (https://github.com/yuriysydor1991/darknetxx) keeps a network
 * in the dxxwjz2 file, the gzip compressed JSON document of the text of the
 * network cfg file, the class names at their indexes, the record of the
 * training and the weights. The document of the network alone - of no
 * weights and of the empty training record - is the network to train from
 * scratch, the very one the darknetxx converter makes of a cfg file and of a
 * class names file:
 *
 * @code
 * {
 *   "format": "dxxwjz2",
 *   "version": 2,
 *   "generator": "ImagesAnnotatorDataExporters-0.14 0.14.0",
 *   "network": {"cfg": "[net]\nbatch=64\n...", "classes": [
 *     {"index": 0, "name": "cat"},
 *     {"index": 1, "name": "dog"}
 *   ]},
 *   "training": {"seconds": 0,
 *     "iteration": [],
 *     "loss": [],
 *     "avg_loss": []
 *   }
 * }
 * @endcode
 *
 * The text is laid out the way the darknetxx writes it - a class on a line, a
 * column of the training record on a line - so the converter of the darknetxx
 * turns the file into the same text and nothing but the generator tells the
 * two apart. The document carries neither the header nor the layers of the
 * weights: the darknetxx writes neither of them into the file of the network
 * alone either.
 *
 * The writers of the later darknetxx documents - the dxxwjz3 network and the
 * dxxwjz2p and the dxxwjz3p projects - are the descendants of this one: they
 * name their own format and version and write their own network member, the
 * training record and the members after it.
 */
class Dxxwjz2NetworkWriter
{
 public:
  /// @brief The value of the format key of the dxxwjz2 files.
  inline static const std::string formatName{"dxxwjz2"};
  /// @brief The value of the version key, the darknetxx readers refuse every
  /// other one.
  inline static const int formatVersion{2};
  /// @brief The file name extension the darknetxx tells the format by.
  inline static const std::string extension{".dxxwjz2"};

  /**
   * @param destination The stream the JSON text goes into.
   * @param cfgText The text of the network cfg file, which the darknetxx
   * refuses to be empty.
   * @param classNames The class names in the order of their indexes.
   */
  Dxxwjz2NetworkWriter(std::ostream& destination, const std::string& cfgText,
                       const std::vector<std::string>& classNames);
  virtual ~Dxxwjz2NetworkWriter() = default;

  /**
   * @brief Writes the whole JSON document into the destination stream.
   *
   * @return Returns false, with nothing written, for the cfg text the
   * document can't keep (see the keeps_cfg()).
   */
  bool write();

  /// @brief The program the generator key names as the writer of the file:
  /// the name and the version of this library.
  static std::string generator();

  /**
   * @brief The JSON string text of any text, quoted and escaped the way the
   * darknetxx does it. The JSON text is UTF-8, which the darknetxx readers
   * insist on, so every byte of the text which isn't a part of a valid UTF-8
   * sequence (the Latin-1 comment of a cfg file, for example) becomes the
   * U+FFFD replacement character.
   */
  static std::string quote(const std::string& text);

 protected:
  /// @brief Tells the document keeps the cfg text: the darknetxx refuses the
  /// network description of no cfg text.
  virtual bool keeps_cfg() const;

  /// @brief The value of the format key of the document.
  virtual const std::string& format_name() const;
  /// @brief The value of the version key of the document.
  virtual int format_version() const;

  /// @brief Writes the network member: the cfg text and the class objects.
  virtual void network();
  /// @brief Writes the empty training record member.
  virtual void training();
  /// @brief Writes the members after the training record one, none of the
  /// dxxwjz2 document.
  virtual void members();

  std::ostream& stream;
  const std::string& cfg;
  const std::vector<std::string>& classes;

 private:
  /// @brief The length of the valid UTF-8 sequence (RFC 3629) starting at the
  /// position, 0 for none.
  static std::size_t utf8_sequence(const std::string& text,
                                   std::size_t position);

  static void escape(char symbol, std::string& quoted);
};

}  // namespace iannotator::exporters

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ2NETWORKWRITER_CLASS_H
