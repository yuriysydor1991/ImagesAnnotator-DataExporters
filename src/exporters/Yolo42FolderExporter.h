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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_YOLO42FOLDEREXPORTER_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_YOLO42FOLDEREXPORTER_CLASS_H

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <utility>

#include "src/exporters/ExportersAliases.h"
#include "src/helpers/ImageLoader.h"
#include "src/helpers/TypeHelper.h"

namespace iannotator::exporters
{

/**
 * @brief The annotations to yolo4 exporter class.
 */
class Yolo42FolderExporter : virtual public IExporter,
                             virtual public helpers::TypeHelper
{
 public:
  virtual ~Yolo42FolderExporter() = default;
  Yolo42FolderExporter() = default;

  virtual bool export_db(LibraryContextPtr) override;

 private:
  using AnnotationsList = IImagesPathsDBProvider::AnnotationsList;
  using DataImage2TxtRec = std::pair<std::string, std::string>;
  using IndexType = AnnotationsList::difference_type;

  inline static const std::string dataRel = "data";
  inline static const std::string cfgRel = "cfg";
  inline static const std::string backupRel = "backup";
  inline static const std::string objNamesRel = dataRel + "/obj.names";
  inline static const std::string objDataRel = dataRel + "/obj.data";
  inline static const std::string trainTxtRel = dataRel + "/train.txt";
  inline static const std::string valTxtRel = dataRel + "/val.txt";
  inline static const std::string yolov4CfgRel = cfgRel + "/yolov4-obj.cfg";

  static bool create_subdirs(LibraryContextPtr ectx);
  bool express_obj_names(LibraryContextPtr ectx);
  bool express_obj_data(LibraryContextPtr ectx);
  bool express_yolocfg(LibraryContextPtr ectx);
  bool express_train_and_val(LibraryContextPtr ectx);
  DataImage2TxtRec prepare_image(LibraryContextPtr ectx, ImageRecordPtr& ir);
  bool express_image_annotations(LibraryContextPtr ectx, ImageRecordPtr& ir,
                                 const std::string& irtxtpath);
  static bool express_rectangle_data(std::fstream& ftxt, ImageRecordPtr& ir,
                                     const IndexType& index,
                                     const ImageRecordRectPtr& irr);
  static std::filesystem::path get_new_filepath(LibraryContextPtr ectx,
                                                ImageRecordPtr& ir);

  AnnotationsList aList;
  helpers::ImageLoaderPtr irloader;
  /// @brief The images the export has put into the training list
  std::size_t exportedImages{0};
};

}  // namespace iannotator::exporters

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_YOLO42FOLDEREXPORTER_CLASS_H
