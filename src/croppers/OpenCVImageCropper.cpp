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

#include "src/croppers/OpenCVImageCropper.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <memory>
#include <string>

#include "src/log/log.h"

namespace iannotator::exporters::croppers
{

namespace
{

namespace fs = std::filesystem;

/// @brief The format every crop is written in, as the consumer supplied
/// croppers of the ImagesAnnotator application write it too.
const std::string cropExtension = ".png";

/**
 * @brief Forces the .png extension onto the wanted path and, when that name is
 * taken, walks a -N counter until a free one is found.
 */
std::string free_png_path(const fs::path& wanted)
{
  const fs::path base = wanted.parent_path() / wanted.stem();

  fs::path candidate = base;
  candidate += cropExtension;

  for (unsigned long long iter = 0U; fs::exists(candidate); ++iter) {
    candidate = base;
    candidate += "-" + std::to_string(iter) + cropExtension;
  }

  return candidate.string();
}

}  // namespace

bool OpenCVImageCropper::crop_out_2_fs(ImageRecordPtr ir,
                                       ImageRecordRectPtr irr,
                                       std::string& tofpath)
{
  assert(ir != nullptr);
  assert(irr != nullptr);
  assert(!tofpath.empty());

  if (ir == nullptr || irr == nullptr) {
    LOGE("Invalid image or rectangle record provided");
    return false;
  }

  if (tofpath.empty()) {
    LOGE("New image destination path is empty");
    return false;
  }

  const std::string srcPath = ir->get_full_path();

  if (srcPath.empty()) {
    LOGE("Invalid filepath obtained from the image record");
    return false;
  }

  const cv::Mat image = cv::imread(srcPath, cv::IMREAD_UNCHANGED);

  if (image.empty()) {
    LOGE("Fail to decode the image: " << srcPath);
    return false;
  }

  // Clamped rather than rejected: an annotation reaching over an edge yields
  // the overlapping part, which is what the consumer supplied croppers of the
  // ImagesAnnotator application produce for it as well.
  const int nx = std::max(0, irr->x);
  const int ny = std::max(0, irr->y);
  const int nwidth = std::min(irr->width, image.cols - nx);
  const int nheight = std::min(irr->height, image.rows - ny);

  if (nwidth <= 0 || nheight <= 0) {
    LOGE("The rectangle " << irr->name << " lies outside of " << srcPath);
    return false;
  }

  const cv::Mat crop = image(cv::Rect{nx, ny, nwidth, nheight}).clone();

  tofpath = free_png_path(fs::path{tofpath});

  if (!cv::imwrite(tofpath, crop)) {
    LOGE("Fail to write the crop: " << tofpath);
    return false;
  }

  LOGT("Cropped " << irr->name << " of " << srcPath << " into " << tofpath);

  return true;
}

IImageCropperFacilityPtr OpenCVImageCropper::clone()
{
  return std::make_shared<OpenCVImageCropper>();
}

}  // namespace iannotator::exporters::croppers
