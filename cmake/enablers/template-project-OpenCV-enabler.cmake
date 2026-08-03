cmake_minimum_required(VERSION 3.13)

# The library decodes no image format of its own and asks its consumer for an
# IImageCropperFacility. This module makes OpenCV available so that it can also
# ship one itself, for consumers that have no imaging stack to lend.
#
# Taken from the appOpenCV branch of the project template, with one deliberate
# difference: there the dependency is mandatory and the module ends in a
# find_package(OpenCV REQUIRED ...), which fails the configure on a system
# without OpenCV. Here it is optional. A missing OpenCV only means the library
# is built without its own cropper, and the PyTorch Vision export keeps asking
# the consumer for one, exactly as it did before this cropper existed. The
# component list is the template's reduced to what the cropper actually calls:
# it decodes, cuts and encodes, so core and imgcodecs, and neither the
# objdetect nor the imgproc of the template.
#
# This module only makes the dependency AVAILABLE. It runs before the library
# target exists, so that the unit test executables declared inside src/ may
# already link against it. The linking against the library target itself is
# performed later by the companion template-project-OpenCV-linker module.
option(
  ENABLE_OPENCV
  "Set to ON to probe the system OpenCV and build the library's own image cropper with it"
  ON
)

# The one variable the rest of the build asks: src/lib/libmain/croppers adds the
# cropper source only when it is ON, and the linker module below links OpenCV
# and defines IADE_WITH_OPENCV for the compilation.
set(IADE_WITH_OPENCV OFF)

if (NOT ENABLE_OPENCV)
  message(STATUS
    "OpenCV integration is OFF - the library ships no image cropper of its "
    "own and every consumer has to supply one")
  return()
endif()

set(
  TEMPLATE_APP_OPENCV_COMPONENTS
  "core;imgcodecs"
  CACHE STRING
  "Semicolon-separated list of OpenCV components required by the project"
)

find_package(OpenCV QUIET COMPONENTS ${TEMPLATE_APP_OPENCV_COMPONENTS})

if (NOT OpenCV_FOUND)
  message(STATUS
    "OpenCV was not found - the library ships no image cropper of its own; "
    "the PyTorch Vision export will keep asking its consumer for one. Install "
    "the OpenCV development packages, or point the configure at them with "
    "-DOpenCV_DIR=<dir>, to build it.")
  return()
endif()

set(IADE_WITH_OPENCV ON)

message(STATUS "OpenCV ${OpenCV_VERSION} found at ${OpenCV_INCLUDE_DIRS}")
