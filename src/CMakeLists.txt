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
# One sub-directory per dataset layout, so a unit test names the sources of
# its own layout only, while the whole list stays available for the tests
# reaching every exporter at once.
set(IADE_EXPORTERS_SRC_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/exporters
)

set(IADE_PLAIN_TXT_SRC
  ${IADE_EXPORTERS_SRC_DIR}/PlainTxt/PlainTxt2FolderExporter.cpp
)

set(IADE_YOLO4_SRC
  ${IADE_EXPORTERS_SRC_DIR}/Yolo4/Yolo42FolderExporter.cpp
  ${IADE_EXPORTERS_SRC_DIR}/Yolo4/Yolov4CfgWriter.cpp
)

set(IADE_PYTORCH_SRC
  ${IADE_EXPORTERS_SRC_DIR}/PyTorch/PyTorchVisionFolderExporter.cpp
)

set(IADE_EXPORTERS_SRC
  ${IADE_PLAIN_TXT_SRC}
  ${IADE_YOLO4_SRC}
  ${IADE_PYTORCH_SRC}
)

# The installed contexts carry the accessors of the export data, so every test
# executable driving a context needs these translation units, exactly as the
# library target itself does.
set(IADE_FACADE_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/facade/LibraryContext.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/facade/PyTorchExportLibraryContext.cpp
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
