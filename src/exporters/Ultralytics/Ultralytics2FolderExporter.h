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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_ULTRALYTICS2FOLDEREXPORTER_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_ULTRALYTICS2FOLDEREXPORTER_CLASS_H

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

#include "src/exporters/ExportersAliases.h"
#include "src/helpers/ImageLoader.h"
#include "src/helpers/TypeHelper.h"

namespace iannotator::exporters
{

/**
 * @brief The directory layout every Ultralytics YOLO dataset shares: the
 * data.yaml descriptor, the images/train images and the labels/train label
 * files next to them.
 *
 * The layout is the one YOLO v5 introduced and every Ultralytics release since
 * - v8, v11 and the ones after them - kept, whatever the trained task. What the
 * task does change is the single label file line of a rectangle, which is why
 * that line is the one thing this class leaves to its descendants. See
 * UltralyticsDetect2FolderExporter and UltralyticsObb2FolderExporter.
 *
 * The darknet directory of Yolo42FolderExporter is a different layout
 * altogether: it names its classes in data/obj.names instead of the descriptor,
 * lists its images in data/train.txt instead of holding them in a directory of
 * their own, and carries the whole network in cfg/yolov4-obj.cfg.
 */
class Ultralytics2FolderExporter : virtual public IExporter,
                                   virtual public helpers::TypeHelper
{
 public:
  virtual ~Ultralytics2FolderExporter() = default;
  Ultralytics2FolderExporter() = default;

  virtual bool export_db(LibraryContextPtr) override;

 protected:
  using AnnotationsList = IImagesPathsDBProvider::AnnotationsList;
  using IndexType = AnnotationsList::difference_type;

  /**
   * @brief One rectangle of a record, clamped into the image and divided by
   * the image dimensions, so every field lies in the 0..1 range the YOLO label
   * files are written in.
   */
  struct NormalizedRect
  {
    double left{0.0};
    double top{0.0};
    double right{0.0};
    double bottom{0.0};
  };

  /**
   * @brief Writes the single label file line the implemented task expects for
   * one rectangle of a record.
   *
   * @param ftxt The open label file of the image the rectangle belongs to.
   * @param index The class index of the rectangle annotation name.
   * @param nrect The rectangle, already clamped and normalised.
   */
  virtual void express_rectangle_data(std::fstream& ftxt,
                                      const IndexType& index,
                                      const NormalizedRect& nrect) const = 0;

  /**
   * @brief Writes the class index followed by the four corners of the
   * rectangle, clockwise from the top left one.
   */
  static void express_corners(std::fstream& ftxt, const IndexType& index,
                              const NormalizedRect& nrect);

 private:
  inline static const std::string imagesRel = "images/train";
  inline static const std::string labelsRel = "labels/train";
  inline static const std::string dataYamlRel = "data.yaml";

  static bool create_subdirs(LibraryContextPtr ectx);
  bool express_data_yaml(LibraryContextPtr ectx) const;
  bool express_images(LibraryContextPtr ectx);
  std::filesystem::path prepare_image(LibraryContextPtr ectx,
                                      ImageRecordPtr& ir);
  bool express_image_annotations(ImageRecordPtr& ir,
                                 const std::filesystem::path& irtxtpath) const;
  static bool normalize(const ImageRecordPtr& ir, const ImageRecordRectPtr& irr,
                        NormalizedRect& nrect);
  static std::filesystem::path get_new_filepath(LibraryContextPtr ectx,
                                                ImageRecordPtr& ir);
  /// @brief Wraps a value into the YAML single quoted style, so an annotation
  /// name or a path holding a colon, a hash or a quote stays one scalar
  static std::string yaml_quoted(const std::string& value);

  AnnotationsList aList;
  helpers::ImageLoaderPtr irloader;
};

}  // namespace iannotator::exporters

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_ULTRALYTICS2FOLDEREXPORTER_CLASS_H
