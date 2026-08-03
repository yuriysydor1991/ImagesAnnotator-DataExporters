cmake_minimum_required(VERSION 3.13)

# Companion of the template-project-OpenCV-enabler module: links the already
# made available OpenCV against the project library target, which only exists
# once src/ has been added.

if (NOT IADE_WITH_OPENCV)
  return()
endif()

# PRIVATE on purpose: no installable public header of the library exposes an
# OpenCV type. The cropper built with it is reached through the abstract
# IImageCropperFacility like any consumer supplied one, so a consuming project
# needs no OpenCV of its own to use this library.
target_link_libraries(
  ${PROJECT_LIBRARY_NAME}
  PRIVATE ${OpenCV_LIBS}
)

# What the always compiled croppers/ImageCropperFactory.cpp branches on to
# decide whether there is a built-in cropper to hand out.
target_compile_definitions(
  ${PROJECT_LIBRARY_NAME}
  PRIVATE IADE_WITH_OPENCV=1
)
