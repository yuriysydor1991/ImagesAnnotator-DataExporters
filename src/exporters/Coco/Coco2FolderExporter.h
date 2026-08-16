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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_COCO2FOLDEREXPORTER_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_COCO2FOLDEREXPORTER_CLASS_H

#include <filesystem>
#include <memory>
#include <sstream>
#include <string>

#include "src/exporters/ExportersAliases.h"
#include "src/helpers/ImageLoader.h"

namespace iannotator::exporters
{

/**
 * @brief The COCO object detection dataset exporter: the images directory and
 * the single annotations/instances_default.json descriptor over it.
 *
 * The descriptor is the one file every reader of the format asks for -
 * Detectron2, MMDetection, torchvision, the HuggingFace detection
 * transformers, CVAT, FiftyOne, Label Studio and Roboflow among them - and it
 * names its images by their file name alone, so the images directory next to
 * it is what such a reader is given as the dataset root.
 *
 * Unlike every YOLO layout of this library, the format carries a rectangle in
 * the image own pixels: the `bbox` is the `[x, y, width, height]` of the top
 * left corner the ImageRecordRect already holds. What the exporter does to it
 * is sort the edges of a negative sized rectangle and cut the result down to
 * the image, since the `area` written next to the box, and every overlap ever
 * computed against it, would otherwise be a lie.
 *
 * The identifiers of the three arrays are the running counters of what the
 * export has really written out, starting at 1: a reader treats the category 0
 * as the background one.
 */
class Coco2FolderExporter : virtual public IExporter
{
 public:
  virtual ~Coco2FolderExporter() = default;
  Coco2FolderExporter() = default;

  virtual bool export_db(LibraryContextPtr) override;

 private:
  using AnnotationsList = IImagesPathsDBProvider::AnnotationsList;
  /// @brief The COCO identifier of an image, an annotation or a category
  using IdType = unsigned long long;

  /**
   * @brief One rectangle of a record, sorted and cut down to the image, in the
   * image own pixels the COCO bbox is written in.
   */
  struct PixelRect
  {
    int left{0};
    int top{0};
    int width{0};
    int height{0};
  };

  inline static const std::string imagesRel = "images";
  inline static const std::string annotationsRel = "annotations";
  inline static const std::string instancesJsonRel =
      annotationsRel + "/instances_default.json";

  static bool create_subdirs(LibraryContextPtr ectx);
  bool express_images(LibraryContextPtr ectx);
  std::filesystem::path prepare_image(LibraryContextPtr ectx,
                                      ImageRecordPtr& ir);
  void express_image(const ImageRecordPtr& ir, const IdType& imageId,
                     const std::string& fileName);
  void express_image_annotations(const ImageRecordPtr& ir,
                                 const IdType& imageId);
  bool express_instances_json(LibraryContextPtr ectx) const;
  std::string express_categories() const;
  static std::filesystem::path get_new_filepath(LibraryContextPtr ectx,
                                                ImageRecordPtr& ir);
  static bool clamp_into_image(const ImageRecordPtr& ir,
                               const ImageRecordRectPtr& irr, PixelRect& prect);
  /// @brief Opens the next element of a JSON array under construction: the
  /// comma every element but the first one follows its predecessor with, and
  /// the line of its own each of them is written on
  static std::ostream& next_element(std::ostringstream& array);
  /// @brief Wraps a value into the JSON string quotes, so an annotation name
  /// or a file name holding a quote, a backslash or a control character stays
  /// one string
  static std::string json_quoted(const std::string& value);

  AnnotationsList aList;
  helpers::ImageLoaderPtr irloader;
  /// @brief The elements of the two arrays the descriptor is built of,
  /// collected while the images are copied out
  std::ostringstream imagesJson;
  std::ostringstream annotationsJson;
  IdType annotationId{0};
};

}  // namespace iannotator::exporters

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_COCO2FOLDEREXPORTER_CLASS_H
