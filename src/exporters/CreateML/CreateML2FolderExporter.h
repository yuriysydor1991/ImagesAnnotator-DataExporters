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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_CREATEML2FOLDEREXPORTER_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_CREATEML2FOLDEREXPORTER_CLASS_H

#include <filesystem>
#include <memory>
#include <sstream>
#include <string>

#include "src/exporters/ExportersAliases.h"
#include "src/helpers/ImageLoader.h"

namespace iannotator::exporters
{

/**
 * @brief The Create ML object detection dataset exporter: the images and the
 * single annotations.json descriptor beside them, in one flat directory.
 *
 * This is the route from a project to a Core ML detector running on an iPhone.
 * The layout is the `directoryWithImagesAndJsonAnnotation` data source of
 * Apple's MLObjectDetector - the very one the Create ML application itself
 * takes when a folder is dropped into its training well - which is why the
 * images are not moved into a subdirectory the way every other layout of this
 * library arranges them: the descriptor names an image by its file name alone
 * and the folder holding both is what is handed over.
 *
 * That data source asks for **exactly one** JSON file in the directory, so the
 * export keeps the annotations.json name for itself and suffixes any image
 * that would otherwise take it.
 *
 * The rectangle is written the way `MLObjectDetector.DataSource.boundingBox`
 * reads it with the defaults the Create ML application uses - `units: .pixel`,
 * `origin: .topLeft`, `anchor: .center` - so `x`/`y` is the **centre** of the
 * box and not its corner, counted in the image own pixels from its top left.
 * That centre is the halving every YOLO layout of this library performs too,
 * only left in pixels here instead of normalised away. As the COCO exporter
 * does, this one sorts the edges of a negative sized rectangle and cuts the
 * result down to the image before the centre is taken from it, since a centre
 * computed over an uncut box does not lie inside the box a reader sees.
 *
 * Nothing here is numbered: the class name reaches the descriptor as it
 * stands, so the position of a name in the available annotations list decides
 * nothing.
 */
class CreateML2FolderExporter : virtual public IExporter
{
 public:
  virtual ~CreateML2FolderExporter() = default;
  CreateML2FolderExporter() = default;

  virtual bool export_db(LibraryContextPtr) override;

 private:
  /**
   * @brief One rectangle of a record, sorted and cut down to the image, in the
   * image own pixels the Create ML coordinates are written in.
   */
  struct PixelRect
  {
    int left{0};
    int top{0};
    int width{0};
    int height{0};
  };

  /// @brief The one JSON file the data source allows the directory to hold
  inline static const std::string annotationsRel = "annotations.json";

  static bool create_export_dir(LibraryContextPtr ectx);
  bool express_images(LibraryContextPtr ectx);
  std::filesystem::path prepare_image(LibraryContextPtr ectx,
                                      ImageRecordPtr& ir);
  void express_image(const ImageRecordPtr& ir, const std::string& fileName);
  static std::string express_annotations(const ImageRecordPtr& ir);
  bool express_annotations_json(LibraryContextPtr ectx) const;
  static std::filesystem::path get_new_filepath(LibraryContextPtr ectx,
                                                ImageRecordPtr& ir);
  static bool clamp_into_image(const ImageRecordPtr& ir,
                               const ImageRecordRectPtr& irr, PixelRect& prect);
  /// @brief Opens the next element of a JSON array under construction: the
  /// comma every element but the first one follows its predecessor with, and
  /// the line of its own each of them is written on
  static std::ostream& next_element(std::ostringstream& array,
                                    const std::string& indent);
  /// @brief Wraps a value into the JSON string quotes, so an annotation name
  /// or a file name holding a quote, a backslash or a control character stays
  /// one string
  static std::string json_quoted(const std::string& value);

  helpers::ImageLoaderPtr irloader;
  /// @brief The elements of the descriptor array, one per image, collected
  /// while the images are copied out
  std::ostringstream imagesJson;
};

}  // namespace iannotator::exporters

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_CREATEML2FOLDEREXPORTER_CLASS_H
