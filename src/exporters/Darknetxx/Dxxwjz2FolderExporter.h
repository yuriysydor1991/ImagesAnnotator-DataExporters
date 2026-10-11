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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ2FOLDEREXPORTER_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ2FOLDEREXPORTER_CLASS_H

#include <ostream>
#include <string>
#include <vector>

#include "src/exporters/ExportersAliases.h"
#include "src/exporters/Yolo4/Yolo42FolderExporter.h"

namespace iannotator::exporters
{

/**
 * @brief The darknetxx training directory exporter: the YOLO v4 (darknet) one
 * with the network to train beside its cfg file.
 *
 * The darknetxx (https://github.com/yuriysydor1991/darknetxx) trains the very
 * directory the Yolo42FolderExporter writes - its darknetxx-gui takes the
 * `cfg/yolov4-obj.cfg` network, the `data/obj.names` class names, the
 * `data/train.txt` images list and resumes from the newest weights of the
 * `backup/` directory - and keeps the networks it trains in the dxxwjz2
 * files. This exporter writes that directory and adds the
 * `cfg/yolov4-obj.dxxwjz2` file to it: the network alone, of no weights,
 * which carries the text of the cfg file next to it and the class names at
 * the indexes the label files number them by.
 *
 * Such a file is what the darknetxx trains from scratch with no other file
 * named, from inside of the exported directory:
 *
 * @code
 * darknetxx --train-cfg cfg/yolov4-obj.dxxwjz2
 * @endcode
 *
 * The class names travel inside of the network then, so the backups the
 * training saves keep them too. The file is named after the cfg one, which is
 * what keeps the backups of a training started either way under one and the
 * same `backup/yolov4-obj_*` name a resumed training looks for.
 *
 * The dxxwjz1 file of the darknetxx is not written: it keeps the weights of a
 * network alone, which an annotations database has none of.
 *
 * The descendants write the network to train of the later darknetxx files
 * into the same directory (see the Dxxwjz3FolderExporter).
 */
class Dxxwjz2FolderExporter : public Yolo42FolderExporter
{
 public:
  virtual ~Dxxwjz2FolderExporter() = default;
  Dxxwjz2FolderExporter() = default;

  virtual bool export_db(IADataExportersContextPtr ectx) override;

 protected:
  /// @brief The file name extension of the network to train, the darknetxx
  /// tells the format of the file by.
  virtual const std::string& network_extension() const;

  /**
   * @brief Writes the JSON document of the network to train: of the cfg text
   * the export has written and of the class names of the export.
   *
   * @return Returns false for the document unable to keep the cfg text.
   */
  virtual bool write_network(std::ostream& document,
                             const std::vector<std::string>& classes) const;

 private:
  bool express_network_to_train(IADataExportersContextPtr ectx) const;
};

}  // namespace iannotator::exporters

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ2FOLDEREXPORTER_CLASS_H
