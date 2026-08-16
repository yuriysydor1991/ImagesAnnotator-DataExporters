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

#include "src/exporters/Ultralytics/Ultralytics2FolderExporter.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>

#include "src/log/log.h"

namespace iannotator::exporters
{

namespace
{
namespace fs = std::filesystem;
}

bool Ultralytics2FolderExporter::export_db(LibraryContextPtr ectx)
{
  assert(ectx != nullptr);

  if (ectx == nullptr) {
    LOGE("Invalid export context pointer provided");
    return false;
  }

  if (ectx->get_export_path().empty()) {
    LOGE("No dst folder export path given");
    return false;
  }

  if (ectx->get_db_provider() == nullptr) {
    LOGE("No annotations database provided");
    return false;
  }

  LOGI("Exporting to " << ectx->get_export_path());

  irloader = helpers::ImageLoader::create();

  aList = ectx->get_db_provider()->get_available_annotations();

  if (!create_subdirs(ectx)) {
    LOGE("Failure while creating necessary directories");
    return false;
  }

  if (!express_data_yaml(ectx)) {
    LOGE("Failure to express the data.yaml file");
    return false;
  }

  if (!express_images(ectx)) {
    LOGE("Failure to copy the images and to express their labels");
    return false;
  }

  return true;
}

bool Ultralytics2FolderExporter::create_subdirs(LibraryContextPtr ectx)
{
  const fs::path dirPath = ectx->get_export_path();

  try {
    // both are nested, so the export directory itself comes along with them
    for (const auto& rel : {imagesRel, labelsRel}) {
      const fs::path subPath = dirPath / rel;

      if (fs::is_directory(subPath)) {
        continue;
      }

      if (!fs::create_directories(subPath)) {
        LOGE("Failure while creating the directory: " << subPath.string());
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

bool Ultralytics2FolderExporter::express_data_yaml(LibraryContextPtr ectx) const
{
  const fs::path fpath = fs::path{ectx->get_export_path()} / dataYamlRel;

  std::fstream yamlFile(fpath.string().c_str(),
                        std::fstream::out | std::fstream::trunc);

  if (!yamlFile.is_open()) {
    LOGE("Failure during file opening " << fpath.string());
    return false;
  }

  if (aList.empty()) {
    LOGW("No annotation classes: YOLO trains no detector of 0 classes");
  }

  std::string root;

  try {
    root = fs::absolute(ectx->get_export_path()).lexically_normal().string();
  }
  catch (const std::exception& e) {
    LOGE("Failure while resolving the export path: " << e.what());
    return false;
  }

  yamlFile << "# The Ultralytics YOLO dataset descriptor, written by the "
              "ImagesAnnotator"
           << std::endl;
  yamlFile << "# annotations dataset exporters library." << std::endl;
  yamlFile << "#" << std::endl;
  yamlFile << "# Drop the path line below to move this directory elsewhere: an "
              "Ultralytics"
           << std::endl;
  yamlFile << "# release then takes the directory holding this very file as "
              "the dataset root."
           << std::endl;
  yamlFile << "path: " << yaml_quoted(root) << std::endl;
  yamlFile << "train: " << imagesRel << std::endl;
  // the whole set is offered for the validation as well, exactly as the
  // darknet layout writes one and the same list into train.txt and val.txt:
  // splitting it into a real training and validation part is the call of the
  // consumer, not of the exporter
  yamlFile << "val: " << imagesRel << std::endl;
  yamlFile << std::endl;
  yamlFile << "names:" << std::endl;

  IndexType index{0};

  for (const auto& name : aList) {
    yamlFile << "  " << index++ << ": " << yaml_quoted(name) << std::endl;
  }

  yamlFile.close();

  return true;
}

bool Ultralytics2FolderExporter::express_images(LibraryContextPtr ectx)
{
  const fs::path labelsPath = fs::path{ectx->get_export_path()} / labelsRel;

  for (auto& ir : ectx->get_db_provider()->get_images_db()) {
    assert(ir != nullptr);

    if (ir == nullptr) {
      LOGE("Invalid image record pointer in the database");
      continue;
    }

    if (ir->rects.empty()) {
      LOGT("Skipping image without annotations: " << ir->get_full_path());
      continue;
    }

    if (ir->iwidth == 0 || ir->iheight == 0) {
      LOGT("Skipping image record with 0 width or height: "
           << ir->get_full_path());
      continue;
    }

    const fs::path newpath = prepare_image(ectx, ir);

    if (newpath.empty()) {
      LOGE("skipping image: " << ir->get_full_path());
      continue;
    }

    const fs::path irtxtpath = labelsPath / (newpath.stem().string() + ".txt");

    if (!express_image_annotations(ir, irtxtpath)) {
      LOGE("Fail to express annotations for " << ir->get_full_path());
      continue;
    }
  }

  return true;
}

fs::path Ultralytics2FolderExporter::prepare_image(LibraryContextPtr ectx,
                                                   ImageRecordPtr& ir)
{
  assert(irloader != nullptr);

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

  assert(!newpath.empty());

  if (newpath.empty()) {
    LOGE("Failure during new file path obtaining");
    return {};
  }

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

  return newpath;
}

fs::path Ultralytics2FolderExporter::get_new_filepath(LibraryContextPtr ectx,
                                                      ImageRecordPtr& ir)
{
  const fs::path origPath = ir->get_full_path();
  const fs::path imagesPath = fs::path{ectx->get_export_path()} / imagesRel;

  fs::path newpath = imagesPath / origPath.filename();

  if (!fs::is_regular_file(newpath)) {
    return newpath;
  }

  LOGT("File with the same name already exists, creating the new one");

  unsigned long long index{1};
  do {
    newpath =
        imagesPath / (origPath.stem().string() + "-" + std::to_string(index++) +
                      origPath.extension().string());
  } while (fs::is_regular_file(newpath));

  return newpath;
}

bool Ultralytics2FolderExporter::express_image_annotations(
    ImageRecordPtr& ir, const fs::path& irtxtpath) const
{
  assert(!irtxtpath.empty());

  std::fstream ftxt(irtxtpath.string().c_str(),
                    std::fstream::out | std::fstream::trunc);

  if (!ftxt.is_open()) {
    LOGE("Fail to open file " << irtxtpath.string());
    return false;
  }

  for (const auto& irr : ir->rects) {
    assert(irr != nullptr);

    if (irr == nullptr) {
      LOGE("Invalid rect pointer in the queue");
      continue;
    }

    const auto classIter = aList.find(irr->name);

    if (classIter == aList.end()) {
      LOGE("Didn't find class name in the list: " << irr->name);
      continue;
    }

    NormalizedRect nrect;

    if (!normalize(ir, irr, nrect)) {
      LOGE("Skipping the rectangle left with no area inside the image: "
           << irr->name);
      continue;
    }

    express_rectangle_data(ftxt, std::distance(aList.begin(), classIter),
                           nrect);
  }

  ftxt.close();

  return true;
}

// An Ultralytics release refuses a whole image over a label coordinate outside
// of the 0..1 range, so a rectangle drawn over an edge is cut down to the
// image here instead of being handed over as it was drawn. A rectangle drawn
// from the right or from the bottom carries a negative size, which is why the
// edges are sorted before they are cut.
bool Ultralytics2FolderExporter::normalize(const ImageRecordPtr& ir,
                                           const ImageRecordRectPtr& irr,
                                           NormalizedRect& nrect)
{
  const double iwidth = toD(ir->iwidth);
  const double iheight = toD(ir->iheight);

  const double x1 = toD(std::min(irr->x, irr->x + irr->width));
  const double x2 = toD(std::max(irr->x, irr->x + irr->width));
  const double y1 = toD(std::min(irr->y, irr->y + irr->height));
  const double y2 = toD(std::max(irr->y, irr->y + irr->height));

  nrect.left = std::clamp(x1, 0.0, iwidth) / iwidth;
  nrect.right = std::clamp(x2, 0.0, iwidth) / iwidth;
  nrect.top = std::clamp(y1, 0.0, iheight) / iheight;
  nrect.bottom = std::clamp(y2, 0.0, iheight) / iheight;

  return nrect.left < nrect.right && nrect.top < nrect.bottom;
}

std::string Ultralytics2FolderExporter::yaml_quoted(const std::string& value)
{
  std::string quoted{"'"};

  for (const char symbol : value) {
    if (symbol == '\'') {
      quoted += '\'';
    }

    quoted += symbol;
  }

  return quoted + "'";
}

}  // namespace iannotator::exporters
