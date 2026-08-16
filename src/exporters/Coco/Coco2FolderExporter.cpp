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

#include "src/exporters/Coco/Coco2FolderExporter.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>

#include "project-global-decls.h"
#include "src/log/log.h"

namespace iannotator::exporters
{

namespace
{
namespace fs = std::filesystem;
}

bool Coco2FolderExporter::export_db(LibraryContextPtr ectx)
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

  if (!express_images(ectx)) {
    LOGE("Failure to copy the images and to collect their annotations");
    return false;
  }

  // the descriptor comes last: it names the images and the rectangles the
  // export has really written out, not the ones it was handed
  if (!express_instances_json(ectx)) {
    LOGE("Failure to express the " << instancesJsonRel << " file");
    return false;
  }

  return true;
}

bool Coco2FolderExporter::create_subdirs(LibraryContextPtr ectx)
{
  const fs::path dirPath = ectx->get_export_path();

  try {
    // both are nested, so the export directory itself comes along with them
    for (const auto& rel : {imagesRel, annotationsRel}) {
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

bool Coco2FolderExporter::express_images(LibraryContextPtr ectx)
{
  IdType imageId{0};

  annotationId = 0;
  imagesJson.str({});
  annotationsJson.str({});

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

    express_image(ir, ++imageId, newpath.filename().string());
    express_image_annotations(ir, imageId);
  }

  return true;
}

fs::path Coco2FolderExporter::prepare_image(LibraryContextPtr ectx,
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

fs::path Coco2FolderExporter::get_new_filepath(LibraryContextPtr ectx,
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

// The file name alone, since a reader is given the images directory itself as
// the dataset root and joins the two on its own
void Coco2FolderExporter::express_image(const ImageRecordPtr& ir,
                                        const IdType& imageId,
                                        const std::string& fileName)
{
  next_element(imagesJson) << "{\"id\": " << imageId
                           << ", \"file_name\": " << json_quoted(fileName)
                           << ", \"width\": " << ir->iwidth
                           << ", \"height\": " << ir->iheight << "}";
}

void Coco2FolderExporter::express_image_annotations(const ImageRecordPtr& ir,
                                                    const IdType& imageId)
{
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

    PixelRect prect;

    if (!clamp_into_image(ir, irr, prect)) {
      LOGE("Skipping the rectangle left with no area inside the image: "
           << irr->name);
      continue;
    }

    // the categories are numbered from 1 in the very order of the sorted
    // annotations list they are written out in
    const IdType categoryId =
        static_cast<IdType>(std::distance(aList.begin(), classIter)) + 1;

    next_element(annotationsJson)
        << "{\"id\": " << ++annotationId << ", \"image_id\": " << imageId
        << ", \"category_id\": " << categoryId << ", \"bbox\": [" << prect.left
        << ", " << prect.top << ", " << prect.width << ", " << prect.height
        << "], \"area\": " << static_cast<long long>(prect.width) * prect.height
        << ", \"iscrowd\": 0, \"segmentation\": []}";
  }
}

bool Coco2FolderExporter::express_instances_json(LibraryContextPtr ectx) const
{
  const fs::path fpath = fs::path{ectx->get_export_path()} / instancesJsonRel;

  std::fstream jsonFile(fpath.string().c_str(),
                        std::fstream::out | std::fstream::trunc);

  if (!jsonFile.is_open()) {
    LOGE("Failure during file opening " << fpath.string());
    return false;
  }

  if (aList.empty()) {
    LOGW(
        "No annotation classes: a detector of 0 categories is trained by "
        "nobody");
  }

  jsonFile << "{" << std::endl;
  jsonFile << "\"info\": {\"description\": \"The ImagesAnnotator annotations "
              "dataset\", \"version\": "
           << json_quoted(project_decls::PROJECT_BUILD_VERSION) << "},"
           << std::endl;
  jsonFile << "\"licenses\": []," << std::endl;
  jsonFile << "\"images\": [" << imagesJson.str() << std::endl
           << "]," << std::endl;
  jsonFile << "\"annotations\": [" << annotationsJson.str() << std::endl
           << "]," << std::endl;
  jsonFile << "\"categories\": [" << express_categories() << std::endl
           << "]" << std::endl;
  jsonFile << "}" << std::endl;

  jsonFile.close();

  return true;
}

std::string Coco2FolderExporter::express_categories() const
{
  std::ostringstream categoriesJson;
  IdType categoryId{0};

  for (const auto& name : aList) {
    next_element(categoriesJson)
        << "{\"id\": " << ++categoryId << ", \"name\": " << json_quoted(name)
        << ", \"supercategory\": \"\"}";
  }

  return categoriesJson.str();
}

// A rectangle drawn from the right or from the bottom carries a negative size,
// which is why the edges are sorted before they are cut down to the image.
bool Coco2FolderExporter::clamp_into_image(const ImageRecordPtr& ir,
                                           const ImageRecordRectPtr& irr,
                                           PixelRect& prect)
{
  const int x1 =
      std::clamp(std::min(irr->x, irr->x + irr->width), 0, ir->iwidth);
  const int x2 =
      std::clamp(std::max(irr->x, irr->x + irr->width), 0, ir->iwidth);
  const int y1 =
      std::clamp(std::min(irr->y, irr->y + irr->height), 0, ir->iheight);
  const int y2 =
      std::clamp(std::max(irr->y, irr->y + irr->height), 0, ir->iheight);

  prect.left = x1;
  prect.top = y1;
  prect.width = x2 - x1;
  prect.height = y2 - y1;

  return prect.width > 0 && prect.height > 0;
}

std::ostream& Coco2FolderExporter::next_element(std::ostringstream& array)
{
  if (array.tellp() > 0) {
    array << ",";
  }

  return array << std::endl << "  ";
}

std::string Coco2FolderExporter::json_quoted(const std::string& value)
{
  std::ostringstream quoted;

  quoted << '"';

  for (const char symbol : value) {
    if (symbol == '"' || symbol == '\\') {
      quoted << '\\' << symbol;
    } else if (static_cast<unsigned char>(symbol) < ' ') {
      quoted << "\\u" << std::hex << std::setw(4) << std::setfill('0')
             << static_cast<int>(static_cast<unsigned char>(symbol));
    } else {
      quoted << symbol;
    }
  }

  quoted << '"';

  return quoted.str();
}

}  // namespace iannotator::exporters
