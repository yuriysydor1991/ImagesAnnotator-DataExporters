cmake_minimum_required(VERSION 3.13)

# The exporters read their image records and their annotations database
# through the ImagesAnnotatorDataDrivers library, so unlike the optional
# third party enablers of the template this dependency is mandatory.
#
# This module only makes the dependency AVAILABLE. It runs before the library
# target exists, so that the unit and component test executables declared
# inside src/ may already link against it. The linking against the library
# target itself is performed later by the companion
# imagesannotator-dataexporters-data-drivers-linker module.

set(
  IADE_DATA_DRIVERS_PACKAGE "ImagesAnnotatorDataDrivers-0.15"
  CACHE STRING
  "The CMake package name of the ImagesAnnotator data drivers library to build against"
)

set(
  IMAGES_ANNOTATOR_DATA_DRIVERS_TARGET
  "${IADE_DATA_DRIVERS_PACKAGE}::${IADE_DATA_DRIVERS_PACKAGE}"
)

# No version is requested on purpose: the 0.15.1 release of the data drivers
# library installs its package Config file without the companion
# <package>ConfigVersion.cmake, so a versioned find_package() would never
# match it. The package name already carries the major and the minor version.
# A project which takes the data drivers in by the FetchContent as well made
# their target available already.
if(NOT TARGET ${IMAGES_ANNOTATOR_DATA_DRIVERS_TARGET})
  find_package(${IADE_DATA_DRIVERS_PACKAGE} REQUIRED CONFIG)
endif()

message(STATUS "ImagesAnnotator data drivers target: ${IMAGES_ANNOTATOR_DATA_DRIVERS_TARGET}")
