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

#include "src/exporters/Darknetxx/Dxxwjz2FolderExporter.h"

#include <cassert>
#include <filesystem>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "src/exporters/Darknetxx/Dxxwjz2NetworkWriter.h"
#include "src/log/log.h"
#include "src/zlib/ZlibController.h"

namespace iannotator::exporters
{

namespace
{
namespace fs = std::filesystem;
}

bool Dxxwjz2FolderExporter::export_db(IADataExportersContextPtr ectx)
{
  assert(ectx != nullptr);
  assert(!ectx->get_export_path().empty());
  assert(ectx->get_db_provider() != nullptr);

  // the very directory the darknetxx-gui trains, the cfg text and the class
  // names of which the network to train carries over
  if (!Yolo42FolderExporter::export_db(ectx)) {
    return false;
  }

  if (!express_network_to_train(ectx)) {
    LOGE("Failure to express the " << network_extension()
                                   << " network to train");
    return false;
  }

  return true;
}

bool Dxxwjz2FolderExporter::express_network_to_train(
    IADataExportersContextPtr ectx) const
{
  assert(ectx != nullptr);
  assert(!ectx->get_export_path().empty());

  // the darknetxx refuses the network description of no cfg text
  if (yoloCfg.empty()) {
    LOGE("No network cfg text to describe the network to train with");
    return false;
  }

  const fs::path fpath =
      fs::path{ectx->get_export_path()} /
      fs::path{yolov4CfgRel}.replace_extension(network_extension());

  // the very order the label files number the classes in
  const std::vector<std::string> classes{aList.begin(), aList.end()};

  std::ostringstream document;

  if (!write_network(document, classes)) {
    LOGE("The network cfg text the document can't keep");
    return false;
  }

  LOGT("Writing the network to train " << fpath.string());

  const std::string text = document.str();

  return iade0zlibi::ZlibController::create()->compress_to_file(
      fpath.string(), {text.cbegin(), text.cend()});
}

const std::string& Dxxwjz2FolderExporter::network_extension() const
{
  return Dxxwjz2NetworkWriter::extension;
}

bool Dxxwjz2FolderExporter::write_network(
    std::ostream& document, const std::vector<std::string>& classes) const
{
  return Dxxwjz2NetworkWriter{document, yoloCfg, classes}.write();
}

}  // namespace iannotator::exporters
