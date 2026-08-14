cmake_minimum_required(VERSION 3.13)

include(template-project-compile-options)
add_compile_options(${EXTRA_COMPILE_OPTIONS})

configure_file(
  project-global-decls.h.in
  ${CMAKE_BINARY_DIR}/project-global-decls.h
)

set(GENERAL_MOCKS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks")

# Per-component source lists shared with the test executables. The unit tests
# compile the components straight from the tree (against the mocks under
# src/tests/mocks) instead of linking the produced shared library, so they need
# to name the very same translation units. Declared here, before any
# subdirectory is added, so that every test tree below can see them.
set(IADE_EXPORTERS_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/exporters/PlainTxt2FolderExporter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/exporters/Yolo42FolderExporter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/exporters/Yolov4CfgWriter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/exporters/PyTorchVisionFolderExporter.cpp
)

set(IADE_HELPERS_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/helpers/ImageRecordUrlAndPathHelper.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/helpers/ImageLoader.cpp
)

set(IADE_CURL_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/CURL/CURLController.cpp
)

# The cropper factory is compiled into every build; the OpenCV implementation
# behind it only into the ones that found OpenCV. Any test executable that
# compiles the PyTorch Vision exporter needs this list, because that exporter
# asks the factory for a cropper when the context carries none.
set(IADE_CROPPERS_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/croppers/ImageCropperFactory.cpp
)

# What such a test executable additionally needs to compile and link. All three
# are empty in a build without OpenCV, so the test declarations stay free of
# the build time condition.
set(IADE_OPENCV_TEST_INCLUDES "")
set(IADE_OPENCV_TEST_LIBS "")
set(IADE_OPENCV_TEST_DEFS "")

if (IADE_WITH_OPENCV)
  list(APPEND IADE_CROPPERS_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/croppers/OpenCVImageCropper.cpp)

  set(IADE_OPENCV_TEST_INCLUDES ${OpenCV_INCLUDE_DIRS})
  set(IADE_OPENCV_TEST_LIBS ${OpenCV_LIBS})
  set(IADE_OPENCV_TEST_DEFS IADE_WITH_OPENCV=1)
endif()

set(IADE_PUBLIC_INCLUDE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/facade/public
)

# The lib subdirectory declares the library target, so it comes first: every
# implementation component below registers its sources against that target.
add_subdirectory(lib)

add_subdirectory(helpers)
add_subdirectory(croppers)
add_subdirectory(CURL)
add_subdirectory(exporters)

add_subdirectory(log)
