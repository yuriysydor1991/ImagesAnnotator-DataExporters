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

#include "src/exporters/PascalVoc/PascalVoc2FolderExporter.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
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

bool PascalVoc2FolderExporter::export_db(LibraryContextPtr ectx)
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

  if (!create_subdirs(ectx)) {
    LOGE("Failure while creating necessary directories");
    return false;
  }

  if (!express_images(ectx)) {
    LOGE("Failure to copy the images and to express their descriptors");
    return false;
  }

  // the lists come last: they name the images the export has really written
  // out, not the ones it was handed
  if (!express_image_sets(ectx)) {
    LOGE("Failure to express the " << imageSetsRel << " lists");
    return false;
  }

  return true;
}

bool PascalVoc2FolderExporter::create_subdirs(LibraryContextPtr ectx)
{
  const fs::path dirPath = ectx->get_export_path();

  try {
    // all three are nested, so the export directory itself comes along with
    // them
    for (const auto& rel : {imagesRel, annotationsRel, imageSetsRel}) {
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

bool PascalVoc2FolderExporter::express_images(LibraryContextPtr ectx)
{
  imageSet.str({});

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

    if (!express_annotation(ectx, ir, newpath)) {
      LOGE("Fail to express annotations for " << ir->get_full_path());
      continue;
    }

    imageSet << newpath.stem().string() << std::endl;
  }

  return true;
}

fs::path PascalVoc2FolderExporter::prepare_image(LibraryContextPtr ectx,
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

fs::path PascalVoc2FolderExporter::get_new_filepath(LibraryContextPtr ectx,
                                                    ImageRecordPtr& ir)
{
  const fs::path origPath = ir->get_full_path();
  const fs::path dirPath = ectx->get_export_path();

  fs::path newpath = dirPath / imagesRel / origPath.filename();

  if (!name_taken(dirPath, newpath)) {
    return newpath;
  }

  LOGT("File with the same name already exists, creating the new one");

  unsigned long long index{1};
  do {
    newpath = dirPath / imagesRel /
              (origPath.stem().string() + "-" + std::to_string(index++) +
               origPath.extension().string());
  } while (name_taken(dirPath, newpath));

  return newpath;
}

// The layout keys the descriptor to the image file stem, so a stem another
// extension already took collides just as the whole file name does: `a.png`
// and `a.jpg` would otherwise write one and the same Annotations/a.xml.
bool PascalVoc2FolderExporter::name_taken(const fs::path& dirPath,
                                          const fs::path& newpath)
{
  return fs::is_regular_file(newpath) ||
         fs::is_regular_file(annotation_filepath(dirPath, newpath));
}

fs::path PascalVoc2FolderExporter::annotation_filepath(
    const fs::path& dirPath, const fs::path& imagePath)
{
  return dirPath / annotationsRel / (imagePath.stem().string() + ".xml");
}

// The image is named by its file name and the directory holding it, since a
// devkit reader is given the export directory itself as the dataset root and
// joins the three on its own.
bool PascalVoc2FolderExporter::express_annotation(LibraryContextPtr ectx,
                                                  const ImageRecordPtr& ir,
                                                  const fs::path& newpath)
{
  const fs::path fpath = annotation_filepath(ectx->get_export_path(), newpath);

  std::fstream xmlFile(fpath.string().c_str(),
                       std::fstream::out | std::fstream::trunc);

  if (!xmlFile.is_open()) {
    LOGE("Failure during file opening " << fpath.string());
    return false;
  }

  xmlFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
  xmlFile << "<annotation>" << std::endl;
  xmlFile << "  <folder>" << imagesRel << "</folder>" << std::endl;
  xmlFile << "  <filename>" << xml_escaped(newpath.filename().string())
          << "</filename>" << std::endl;
  xmlFile << "  <source>" << std::endl;
  xmlFile << "    <database>" << databaseName << "</database>" << std::endl;
  xmlFile << "  </source>" << std::endl;
  xmlFile << "  <size>" << std::endl;
  xmlFile << "    <width>" << ir->iwidth << "</width>" << std::endl;
  xmlFile << "    <height>" << ir->iheight << "</height>" << std::endl;
  xmlFile << "    <depth>" << imageDepth << "</depth>" << std::endl;
  xmlFile << "  </size>" << std::endl;
  // the rectangles carry no mask, and a reader takes the flag as the promise
  // that the segmentation directories of the devkit hold one
  xmlFile << "  <segmented>0</segmented>" << std::endl;
  xmlFile << express_objects(ir);
  xmlFile << "</annotation>" << std::endl;

  xmlFile.close();

  return true;
}

std::string PascalVoc2FolderExporter::express_objects(const ImageRecordPtr& ir)
{
  std::ostringstream objects;

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

    objects << "  <object>" << std::endl;
    objects << "    <name>" << xml_escaped(irr->name) << "</name>" << std::endl;
    // neither a viewing angle nor a hard-to-recognize mark is anything an
    // image record holds, and the second one drops an object out of the
    // standard VOC evaluation altogether
    objects << "    <pose>Unspecified</pose>" << std::endl;
    objects << "    <truncated>" << (prect.truncated ? 1 : 0) << "</truncated>"
            << std::endl;
    objects << "    <difficult>0</difficult>" << std::endl;
    objects << "    <bndbox>" << std::endl;
    objects << "      <xmin>" << prect.left << "</xmin>" << std::endl;
    objects << "      <ymin>" << prect.top << "</ymin>" << std::endl;
    objects << "      <xmax>" << prect.right << "</xmax>" << std::endl;
    objects << "      <ymax>" << prect.bottom << "</ymax>" << std::endl;
    objects << "    </bndbox>" << std::endl;
    objects << "  </object>" << std::endl;
  }

  return objects.str();
}

bool PascalVoc2FolderExporter::express_image_sets(LibraryContextPtr ectx) const
{
  const fs::path dirPath = ectx->get_export_path();

  // the whole set is offered for the validation as well, exactly as the
  // darknet layout writes one and the same list into train.txt and val.txt:
  // splitting it into a real training and validation part is the call of the
  // consumer, not of the exporter. The two names themselves are not free
  // either - a reader of the format takes the split to name the list.
  for (const auto& rel : {trainSetRel, valSetRel}) {
    if (!express_image_set(dirPath / rel)) {
      LOGE("Failure to express the " << rel << " file");
      return false;
    }
  }

  return true;
}

bool PascalVoc2FolderExporter::express_image_set(const fs::path& fpath) const
{
  std::fstream setFile(fpath.string().c_str(),
                       std::fstream::out | std::fstream::trunc);

  if (!setFile.is_open()) {
    LOGE("Failure during file opening " << fpath.string());
    return false;
  }

  setFile << imageSet.str();

  setFile.close();

  return true;
}

// A rectangle drawn from the right or from the bottom carries a negative size,
// which is why the edges are sorted before they are cut down to the image.
bool PascalVoc2FolderExporter::clamp_into_image(const ImageRecordPtr& ir,
                                                const ImageRecordRectPtr& irr,
                                                PixelRect& prect)
{
  const int x1 = std::min(irr->x, irr->x + irr->width);
  const int x2 = std::max(irr->x, irr->x + irr->width);
  const int y1 = std::min(irr->y, irr->y + irr->height);
  const int y2 = std::max(irr->y, irr->y + irr->height);

  prect.left = std::clamp(x1, 0, ir->iwidth);
  prect.right = std::clamp(x2, 0, ir->iwidth);
  prect.top = std::clamp(y1, 0, ir->iheight);
  prect.bottom = std::clamp(y2, 0, ir->iheight);
  prect.truncated = prect.left != x1 || prect.right != x2 || prect.top != y1 ||
                    prect.bottom != y2;

  return prect.right > prect.left && prect.bottom > prect.top;
}

std::string PascalVoc2FolderExporter::xml_escaped(const std::string& value)
{
  std::ostringstream escaped;

  for (const char symbol : value) {
    // XML 1.0 carries none of these at all, not even as a numeric reference
    if (static_cast<unsigned char>(symbol) < ' ' && symbol != '\t' &&
        symbol != '\n' && symbol != '\r') {
      continue;
    }

    if (symbol == '&') {
      escaped << "&amp;";
    } else if (symbol == '<') {
      escaped << "&lt;";
    } else if (symbol == '>') {
      escaped << "&gt;";
    } else {
      escaped << symbol;
    }
  }

  return escaped.str();
}

}  // namespace iannotator::exporters
