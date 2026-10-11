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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ2PPROJECTEXPORTER_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ2PPROJECTEXPORTER_CLASS_H

#include <filesystem>
#include <ostream>
#include <string>
#include <vector>

#include "src/exporters/Darknetxx/Dxxwjz2pAnnotationsWriter.h"
#include "src/exporters/ExportersAliases.h"
#include "src/helpers/ImageLoader.h"

namespace iannotator::exporters
{

/**
 * @brief The darknetxx dxxwjz2p training project exporter.
 *
 * The dxxwjz2p file of the darknetxx
 * (https://github.com/yuriysydor1991/darknetxx) is the whole YOLO v4 training
 * project but the images: the network, the class names, the training record
 * and the annotations of the images in one file. The exporter writes the
 * project of the network to train - the YOLO v4 network of the YOLO v4 export
 * of no weights - beside the copies of the annotated images, so the directory
 * moves as a whole:
 *
 * @code
 * <export directory>/
 *   yolov4-obj.dxxwjz2p     the project, of the image paths relative to it
 *   images/                 the copies of the annotated images
 *   backup/                 the weights files the darknetxx training saves
 * @endcode
 *
 * The darknetxx trains it from scratch, from inside of the exported directory
 * (the darknetxx-train-gui opens it as well):
 *
 * @code
 * darknetxx --train-cfg yolov4-obj.dxxwjz2p
 * @endcode
 *
 * Every save of the training updates the project with the weights and the
 * training record, while the backups of the training are named after the
 * project file. The images of the export are the ones of the YOLO v4 export:
 * the ones of the boxes and of a known size.
 *
 * The descendants write the later darknetxx projects of the same directory
 * (see the Dxxwjz3pProjectExporter).
 */
class Dxxwjz2pProjectExporter : virtual public IExporter
{
 public:
  virtual ~Dxxwjz2pProjectExporter() = default;
  Dxxwjz2pProjectExporter() = default;

  virtual bool export_db(IADataExportersContextPtr ectx) override;

 protected:
  using AnnotationsList = IImagesPathsDBProvider::AnnotationsList;
  using Images = Dxxwjz2pAnnotationsWriter::Images;

  /// @brief The name of the project file but its extension, the one of the
  /// network files of the YOLO v4 export: the darknetxx names the backups of
  /// the training after it.
  inline static const std::string projectStem = "yolov4-obj";
  inline static const std::string imagesRel = "images";
  inline static const std::string backupRel = "backup";

  /// @brief The file name extension of the project, the darknetxx tells the
  /// format of the file by.
  virtual const std::string& project_extension() const;

  /**
   * @brief Writes the JSON document of the project.
   *
   * @param document The stream the JSON text goes into.
   * @param cfgText The text of the network cfg.
   * @param classes The class names in the order of their indexes.
   * @param images The images of the images folder.
   *
   * @return Returns false for the document unable to keep the cfg text.
   */
  virtual bool write_project(std::ostream& document, const std::string& cfgText,
                             const std::vector<std::string>& classes,
                             const Images& images) const;

 private:
  static bool create_subdirs(IADataExportersContextPtr ectx);
  Images express_images(IADataExportersContextPtr ectx);
  bool express_project(IADataExportersContextPtr ectx,
                       const Images& images) const;

  /// @brief Copies the image into the images folder, the name of the copy or
  /// an empty one for none.
  std::string prepare_image(IADataExportersContextPtr ectx, ImageRecordPtr& ir);
  static std::filesystem::path get_new_filepath(IADataExportersContextPtr ectx,
                                                ImageRecordPtr& ir);

  /// @brief The class names of the export: the darknet class index of a name
  /// is its position in this sorted set
  AnnotationsList aList;
  helpers::ImageLoaderPtr irloader;
};

}  // namespace iannotator::exporters

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_DXXWJZ2PPROJECTEXPORTER_CLASS_H
