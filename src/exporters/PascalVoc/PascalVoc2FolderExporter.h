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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_PASCALVOC2FOLDEREXPORTER_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_PASCALVOC2FOLDEREXPORTER_CLASS_H

#include <filesystem>
#include <memory>
#include <sstream>
#include <string>

#include "src/exporters/ExportersAliases.h"
#include "src/helpers/ImageLoader.h"

namespace iannotator::exporters
{

/**
 * @brief The Pascal VOC dataset exporter: the JPEGImages directory, one
 * Annotations XML descriptor per image over it and the ImageSets/Main lists
 * naming them.
 *
 * This is the devkit layout torchvision VOCDetection and the MMDetection
 * XMLDataset are pointed at, and the very files LabelImg saves its own work in,
 * so an exported project opens for correction in that annotator directly.
 *
 * A rectangle reaches the XML as the two corner points it was drawn between,
 * in the image own pixels: `xmin`/`ymin` is the top left one and `xmax`/`ymax`
 * the bottom right one, which is the pair LabelImg turns back into the very
 * same rectangle. The exporter sorts the edges of a negative sized rectangle
 * and cuts the result down to the image, drops a rectangle left with no area
 * inside it, and marks the ones the image edge really did cut with the
 * `truncated` flag the format keeps for an object continuing past the image.
 *
 * Unlike every other layout of this library, nothing here is numbered: the
 * class name is written into the XML as it stands, so the position of a name
 * in the available annotations list decides nothing.
 */
class PascalVoc2FolderExporter : virtual public IExporter
{
 public:
  virtual ~PascalVoc2FolderExporter() = default;
  PascalVoc2FolderExporter() = default;

  virtual bool export_db(LibraryContextPtr) override;

 private:
  /**
   * @brief One rectangle of a record, sorted and cut down to the image, as the
   * two corner points the VOC bndbox is written from.
   */
  struct PixelRect
  {
    int left{0};
    int top{0};
    int right{0};
    int bottom{0};
    /// @brief The image edge really did cut the rectangle: what the VOC
    /// truncated flag says about an object continuing past the image
    bool truncated{false};
  };

  inline static const std::string imagesRel = "JPEGImages";
  inline static const std::string annotationsRel = "Annotations";
  inline static const std::string imageSetsRel = "ImageSets/Main";
  inline static const std::string trainSetRel = imageSetsRel + "/train.txt";
  inline static const std::string valSetRel = imageSetsRel + "/val.txt";

  /// @brief What the source of the annotations is named in every descriptor
  inline static const std::string databaseName =
      "The ImagesAnnotator annotations dataset";
  /// @brief An image record carries no channel count, and the format leaves no
  /// way to write the size without one
  inline static constexpr const int imageDepth{3};

  static bool create_subdirs(LibraryContextPtr ectx);
  bool express_images(LibraryContextPtr ectx);
  std::filesystem::path prepare_image(LibraryContextPtr ectx,
                                      ImageRecordPtr& ir);
  static bool express_annotation(LibraryContextPtr ectx,
                                 const ImageRecordPtr& ir,
                                 const std::filesystem::path& newpath);
  static std::string express_objects(const ImageRecordPtr& ir);
  bool express_image_sets(LibraryContextPtr ectx) const;
  bool express_image_set(const std::filesystem::path& fpath) const;
  static std::filesystem::path get_new_filepath(LibraryContextPtr ectx,
                                                ImageRecordPtr& ir);
  /// @brief The descriptor written beside the given copied image
  static std::filesystem::path annotation_filepath(
      const std::filesystem::path& dirPath,
      const std::filesystem::path& imagePath);
  /// @brief Whether the export already holds the image file name, or the
  /// descriptor its stem names
  static bool name_taken(const std::filesystem::path& dirPath,
                         const std::filesystem::path& newpath);
  static bool clamp_into_image(const ImageRecordPtr& ir,
                               const ImageRecordRectPtr& irr, PixelRect& prect);
  /// @brief Replaces the markup characters of a value with their entities, so
  /// an annotation name or a file name holding one of them stays text
  static std::string xml_escaped(const std::string& value);

  helpers::ImageLoaderPtr irloader;
  /// @brief The stems of the images the export has really written out, the
  /// lines of the ImageSets lists, collected while they are copied
  std::ostringstream imageSet;
};

}  // namespace iannotator::exporters

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_PASCALVOC2FOLDEREXPORTER_CLASS_H
