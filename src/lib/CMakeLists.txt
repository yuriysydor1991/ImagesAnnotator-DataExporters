cmake_minimum_required(VERSION 3.13)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_library(
  ${PROJECT_LIBRARY_NAME}
  SHARED
)

target_include_directories(
  ${PROJECT_LIBRARY_NAME}
  PRIVATE ${CMAKE_SOURCE_DIR}
  PRIVATE ${CMAKE_BINARY_DIR}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/facade/public>
    # Both the plain include root and the versioned sub-directory are exported,
    # so a consumer may write either
    #   #include <ImagesAnnotatorDataExporters-0.11/LibraryFacade.h>  (recommended)
    # or the short
    #   #include <LibraryFacade.h>
    # The prefixed form is the safe one: header names such as IExporter.h are
    # generic enough to collide in a busy include path.
    $<INSTALL_INTERFACE:include>
    $<INSTALL_INTERFACE:include/${PROJECT_LIBRARY_NAME}>
)

set_target_properties(
  ${PROJECT_LIBRARY_NAME}
  PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  # Only the IADE_API marked interface of facade/public leaves the shared
  # object. The ImagesAnnotatorDataDrivers library linked below is built from
  # the very same project template and exports its own lib0impl::LibFactory,
  # simple_logger::SimpleLogger and project_decls symbols: exporting ours too
  # would let the dynamic linker bind one library's calls to the other
  # library's definitions, and the two lib0impl::LibFactory classes do not
  # share a vtable layout.
  CXX_VISIBILITY_PRESET hidden
)

target_compile_definitions(
  ${PROJECT_LIBRARY_NAME}
  PRIVATE IMAGES_ANNOTATOR_DATA_EXPORTERS_BUILDING=1
)

# Per-component source lists shared with the test executables. The unit tests
# compile the components straight from the tree (against the mocks under
# src/tests/mocks) instead of linking the produced shared library, so they need
# to name the very same translation units.
set(IADE_EXPORTERS_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/libmain/exporters/PlainTxt2FolderExporter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/libmain/exporters/Yolo42FolderExporter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/libmain/exporters/PyTorchVisionFolderExporter.cpp
)

set(IADE_HELPERS_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/libmain/helpers/ImageRecordUrlAndPathHelper.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/libmain/helpers/ImageLoader.cpp
)

set(IADE_CURL_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/libmain/CURL/CURLController.cpp
)

set(IADE_PUBLIC_INCLUDE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/facade/public
)

add_subdirectory(facade)
add_subdirectory(libmain)

include(lib-cmake-module-gen)
