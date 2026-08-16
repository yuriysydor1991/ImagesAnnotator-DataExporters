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

#include "src/exporters/CreateML/CreateML2FolderExporter.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>

#include "src/log/log.h"

namespace iannotator::exporters
{

namespace
{
namespace fs = std::filesystem;
}

bool CreateML2FolderExporter::export_db(LibraryContextPtr ectx)
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

  if (!create_export_dir(ectx)) {
    LOGE("Failure while creating the export directory");
    return false;
  }

  if (!express_images(ectx)) {
    LOGE("Failure to copy the images and to collect their annotations");
    return false;
  }

  // the descriptor comes last: it names the images and the rectangles the
  // export has really written out, not the ones it was handed
  if (!express_annotations_json(ectx)) {
    LOGE("Failure to express the " << annotationsRel << " file");
    return false;
  }

  return true;
}

// The images and the descriptor share this one directory, so there is no
// subdirectory to make below it
bool CreateML2FolderExporter::create_export_dir(LibraryContextPtr ectx)
{
  const fs::path dirPath = ectx->get_export_path();

  try {
    if (fs::is_directory(dirPath)) {
      return true;
    }

    if (!fs::create_directories(dirPath)) {
      LOGE("Failure while creating the directory: " << dirPath.string());
      return false;
    }
  }
  catch (const std::exception& e) {
    LOGE("Exception during directories create " << e.what());
    return false;
  }

  return true;
}

bool CreateML2FolderExporter::express_images(LibraryContextPtr ectx)
{
  imagesJson.str({});

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

    express_image(ir, newpath.filename().string());
  }

  return true;
}

fs::path CreateML2FolderExporter::prepare_image(LibraryContextPtr ectx,
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

fs::path CreateML2FolderExporter::get_new_filepath(LibraryContextPtr ectx,
                                                   ImageRecordPtr& ir)
{
  const fs::path origPath = ir->get_full_path();
  const fs::path dirPath = ectx->get_export_path();

  // the descriptor shares the directory with the images it names, and the data
  // source reading it accepts no second JSON file beside it
  const auto taken = [](const fs::path& candidate) {
    return fs::is_regular_file(candidate) ||
           candidate.filename() == annotationsRel;
  };

  fs::path newpath = dirPath / origPath.filename();

  if (!taken(newpath)) {
    return newpath;
  }

  LOGT("File with the same name already exists, creating the new one");

  unsigned long long index{1};
  do {
    newpath =
        dirPath / (origPath.stem().string() + "-" + std::to_string(index++) +
                   origPath.extension().string());
  } while (taken(newpath));

  return newpath;
}

// The file name alone, since the directory holding both the images and this
// descriptor is what the data source is pointed at
void CreateML2FolderExporter::express_image(const ImageRecordPtr& ir,
                                            const std::string& fileName)
{
  next_element(imagesJson, "  ")
      << "{\"imagefilename\": " << json_quoted(fileName)
      << ", \"annotation\": [" << express_annotations(ir) << std::endl
      << "  ]}";
}

std::string CreateML2FolderExporter::express_annotations(
    const ImageRecordPtr& ir)
{
  std::ostringstream annotationsJson;

  // A centre is the sum of an integer edge and an integer size halved, so it
  // is either whole or exactly one half and never a value a decimal cannot
  // hold. The default 6 significant digits would still round the half away on
  // an image some tens of thousands of pixels wide, which is the only reason
  // the precision is raised here.
  annotationsJson << std::setprecision(15);

  for (const auto& irr : ir->rects) {
    assert(irr != nullptr);

    if (irr == nullptr) {
      LOGE("Invalid rect pointer in the queue");
      continue;
    }

    PixelRect prect;

    if (!clamp_into_image(ir, irr, prect)) {
      LOGE("Skipping the rectangle left with no area inside the image: "
           << irr->name);
      continue;
    }

    // the centre of the box and not its corner: what
    // MLObjectDetector.DataSource.boundingBox reads with the anchor default
    // the Create ML application uses
    next_element(annotationsJson, "    ")
        << "{\"label\": " << json_quoted(irr->name)
        << ", \"coordinates\": {\"x\": " << (prect.left + prect.width / 2.0)
        << ", \"y\": " << (prect.top + prect.height / 2.0)
        << ", \"width\": " << prect.width << ", \"height\": " << prect.height
        << "}}";
  }

  return annotationsJson.str();
}

bool CreateML2FolderExporter::express_annotations_json(
    LibraryContextPtr ectx) const
{
  const fs::path fpath = fs::path{ectx->get_export_path()} / annotationsRel;

  std::fstream jsonFile(fpath.string().c_str(),
                        std::fstream::out | std::fstream::trunc);

  if (!jsonFile.is_open()) {
    LOGE("Failure during file opening " << fpath.string());
    return false;
  }

  // the whole descriptor is the one array, an element per image
  jsonFile << "[" << imagesJson.str() << std::endl << "]" << std::endl;

  jsonFile.close();

  return true;
}

// A rectangle drawn from the right or from the bottom carries a negative size,
// which is why the edges are sorted before they are cut down to the image: a
// centre taken from an uncut box need not lie inside the box a reader sees.
bool CreateML2FolderExporter::clamp_into_image(const ImageRecordPtr& ir,
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

std::ostream& CreateML2FolderExporter::next_element(std::ostringstream& array,
                                                    const std::string& indent)
{
  if (array.tellp() > 0) {
    array << ",";
  }

  return array << std::endl << indent;
}

std::string CreateML2FolderExporter::json_quoted(const std::string& value)
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
