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

#include "src/exporters/Darknetxx/Dxxwjz2pProjectExporter.h"

#include <cassert>
#include <exception>
#include <filesystem>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "src/exporters/Darknetxx/Dxxwjz2pProjectWriter.h"
#include "src/exporters/Yolo4/Yolov4CfgWriter.h"
#include "src/log/log.h"
#include "src/zlib/ZlibController.h"

namespace iannotator::exporters
{

namespace
{
namespace fs = std::filesystem;
}

bool Dxxwjz2pProjectExporter::export_db(IADataExportersContextPtr ectx)
{
  assert(ectx != nullptr);
  assert(!ectx->get_export_path().empty());
  assert(ectx->get_db_provider() != nullptr);

  LOGI("Exporting to " << ectx->get_export_path());

  irloader = helpers::ImageLoader::create();

  if (!create_subdirs(ectx)) {
    LOGE("Failure while creating necessary directories");
    return false;
  }

  aList = ectx->get_db_provider()->get_available_annotations();

  if (aList.empty()) {
    LOGW("No annotation classes: darknet trains no detector of 0 classes");
  }

  if (!express_project(ectx, express_images(ectx))) {
    LOGE("Failure to express the " << project_extension() << " project");
    return false;
  }

  return true;
}

const std::string& Dxxwjz2pProjectExporter::project_extension() const
{
  return Dxxwjz2pProjectWriter::extension;
}

bool Dxxwjz2pProjectExporter::write_project(
    std::ostream& document, const std::string& cfgText,
    const std::vector<std::string>& classes, const Images& images) const
{
  return Dxxwjz2pProjectWriter{document, cfgText, classes, imagesRel, images}
      .write();
}

bool Dxxwjz2pProjectExporter::create_subdirs(IADataExportersContextPtr ectx)
{
  assert(ectx != nullptr);
  assert(!ectx->get_export_path().empty());

  const fs::path dirPath = ectx->get_export_path();

  try {
    for (const fs::path& subdir : {dirPath / imagesRel, dirPath / backupRel}) {
      if (!fs::is_directory(subdir) && !fs::create_directories(subdir)) {
        LOGE("Failure while creating the directory: " << subdir.string());
        return false;
      }
    }
  }
  catch (const std::exception& e) {
    LOGE("Exception during directories create " << e.what());
    return false;
  }

  return true;
}

Dxxwjz2pProjectExporter::Images Dxxwjz2pProjectExporter::express_images(
    IADataExportersContextPtr ectx)
{
  assert(ectx != nullptr);
  assert(ectx->get_db_provider() != nullptr);

  Images images;

  for (auto& ir : ectx->get_db_provider()->get_images_db()) {
    assert(ir != nullptr);

    if (ir->rects.empty()) {
      LOGT("Skipping image without annotations: " << ir->get_full_path());
      continue;
    }

    // the darknetxx reads the boxes of the images of a known size alone
    if (ir->iwidth <= 0 || ir->iheight <= 0) {
      LOGT("Skipping image record with 0 width or height: "
           << ir->get_full_path());
      continue;
    }

    const std::string name = prepare_image(ectx, ir);

    if (name.empty()) {
      LOGE("skipping image: " << ir->get_full_path());
      continue;
    }

    images.push_back(Dxxwjz2pAnnotationsWriter::Image{name, ir});
  }

  return images;
}

bool Dxxwjz2pProjectExporter::express_project(IADataExportersContextPtr ectx,
                                              const Images& images) const
{
  assert(ectx != nullptr);
  assert(!ectx->get_export_path().empty());

  const std::string projectName = projectStem + project_extension();

  // the network of the YOLO v4 export: the iterations count it is given
  // depends on the images the export has really put into the project
  std::ostringstream cfgText;

  Yolov4CfgWriter{cfgText, aList.size(), images.size(),
                  "darknetxx --train-cfg " + projectName}
      .write();

  // the very order the YOLO v4 label files number the classes in
  const std::vector<std::string> classes{aList.begin(), aList.end()};

  std::ostringstream document;

  if (!write_project(document, cfgText.str(), classes, images)) {
    LOGE("The network cfg text the project can't keep");
    return false;
  }

  const fs::path fpath = fs::path{ectx->get_export_path()} / projectName;

  LOGT("Writing the project " << fpath.string());

  const std::string text = document.str();

  return iade0zlibi::ZlibController::create()->compress_to_file(
      fpath.string(), {text.cbegin(), text.cend()});
}

std::string Dxxwjz2pProjectExporter::prepare_image(
    IADataExportersContextPtr ectx, ImageRecordPtr& ir)
{
  assert(ir != nullptr);
  assert(irloader != nullptr);

  if (ir == nullptr) {
    LOGE("Image record pointer is invalid");
    return {};
  }

  if (!irloader->load(ir)) {
    LOGE("Fail to preload the image: " << ir->get_full_path());
    return {};
  }

  const fs::path origPath = ir->get_full_path();

  if (!fs::is_regular_file(origPath)) {
    LOGE("No file found under the path: " << origPath.string());
    return {};
  }

  const fs::path newpath = get_new_filepath(ectx, ir);

  LOGT("Copying file " << origPath.string() << " to " << newpath.string());

  try {
    if (!fs::copy_file(origPath, newpath)) {
      LOGE("Fail to copy file " << origPath.string());
      return {};
    }
  }
  catch (const std::exception& e) {
    LOGE("failure during file copying " << origPath << " reason: " << e.what());
    return {};
  }

  return newpath.filename().string();
}

fs::path Dxxwjz2pProjectExporter::get_new_filepath(
    IADataExportersContextPtr ectx, ImageRecordPtr& ir)
{
  const fs::path origPath = ir->get_full_path();
  const fs::path imagesDir = fs::path{ectx->get_export_path()} / imagesRel;

  fs::path newpath = imagesDir / origPath.filename();

  if (!fs::exists(newpath)) {
    return newpath;
  }

  LOGT("File with the same name already exists, creating the new one");

  unsigned long long index{1};

  do {
    newpath =
        imagesDir / (origPath.stem().string() + "-" + std::to_string(index++) +
                     origPath.extension().string());
  } while (fs::exists(newpath));

  return newpath;
}

}  // namespace iannotator::exporters
