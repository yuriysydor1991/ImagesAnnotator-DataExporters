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

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_EXPORTERSALIASES_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_EXPORTERSALIASES_H

#include <ImagesAnnotatorDataDrivers-0.11/IImagesPathsDBProvider.h>
#include <ImagesAnnotatorDataDrivers-0.11/ImageRecord.h>
#include <ImagesAnnotatorDataDrivers-0.11/ImageRecordRect.h>

#include "IExporter.h"
#include "IImageCropperFacility.h"
#include "LibraryContext.h"
#include "PyTorchExportLibraryContext.h"

/**
 * @brief The images annotator exporter namespace that holds all annotator
 * related exporters classes.
 *
 * This is the implementation side of the library: the namespace name is the
 * one the code carried inside the ImagesAnnotator application it was moved
 * out of. Nothing declared under it is installed - a consuming project only
 * ever sees the ImagesAnnotatorDataExporters011 interfaces.
 */
namespace iannotator::exporters
{

/**
 * @brief Pulls the installable interface names and the data drivers record
 * names into the implementation namespace, so that the moved exporters code
 * keeps referring to them unqualified the way it did inside the application.
 */
using ImagesAnnotatorDataExporters011::IExporter;
using ImagesAnnotatorDataExporters011::IExporterPtr;
using ImagesAnnotatorDataExporters011::IImageCropperFacility;
using ImagesAnnotatorDataExporters011::IImageCropperFacilityPtr;
using ImagesAnnotatorDataExporters011::LibraryContext;
using ImagesAnnotatorDataExporters011::LibraryContextPtr;
using ImagesAnnotatorDataExporters011::PyTorchExportLibraryContext;

using ImagesAnnotatorDataDrivers011::IImagesPathsDBProvider;
using ImagesAnnotatorDataDrivers011::IImagesPathsDBProviderPtr;
using ImagesAnnotatorDataDrivers011::ImageRecord;
using ImagesAnnotatorDataDrivers011::ImageRecordPtr;
using ImagesAnnotatorDataDrivers011::ImageRecordRect;
using ImagesAnnotatorDataDrivers011::ImageRecordRectPtr;
using ImagesAnnotatorDataDrivers011::ImageRecordRectSet;
using ImagesAnnotatorDataDrivers011::ImageRecordsSet;

}  // namespace iannotator::exporters

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_EXPORTERSALIASES_H
