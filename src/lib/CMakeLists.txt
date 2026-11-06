cmake_minimum_required(VERSION 3.13)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_library(
  ${IADE_LIBRARY_NAME}
  SHARED
)

# The very target name the installed CMake package exports, so a project links
# the same one whether it finds the installed library or takes it in by the
# FetchContent.
add_library(${IADE_LIBRARY_NAME}::${IADE_LIBRARY_NAME} ALIAS ${IADE_LIBRARY_NAME})

# The forwarding headers of the installed paths, so the prefixed includes below
# work against the sources too. The file(GENERATE) rewrites only the changed
# ones, a reconfigure recompiles nothing.
set(IADE_BUILD_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/include")

file(
  GLOB_RECURSE IADE_PUBLIC_HEADERS
  RELATIVE "${IADE_PUBLIC_INCLUDE_DIR}"
  CONFIGURE_DEPENDS "${IADE_PUBLIC_INCLUDE_DIR}/*.h"
)

foreach(publicHeader IN LISTS IADE_PUBLIC_HEADERS)
  file(
    GENERATE
    OUTPUT "${IADE_BUILD_INCLUDE_DIR}/${IADE_LIBRARY_NAME}/${publicHeader}"
    CONTENT "#include \"${IADE_PUBLIC_INCLUDE_DIR}/${publicHeader}\"\n"
  )
endforeach()

target_include_directories(
  ${IADE_LIBRARY_NAME}
  PRIVATE ${PROJECT_SOURCE_DIR}
  PRIVATE ${PROJECT_BINARY_DIR}
  PUBLIC
    # Both the plain include root and the versioned sub-directory are exported,
    # of the build tree and of the installation alike, so a consumer may write
    # either
    #   #include <ImagesAnnotatorDataExporters-0.15/IADataExportersFacade.h>  (recommended)
    # or the short
    #   #include <IADataExportersFacade.h>
    # The prefixed form is the safe one: header names such as IExporter.h are
    # generic enough to collide in a busy include path.
    $<BUILD_INTERFACE:${IADE_BUILD_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${IADE_PUBLIC_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
    $<INSTALL_INTERFACE:include/${IADE_LIBRARY_NAME}>
)

set_target_properties(
  ${IADE_LIBRARY_NAME}
  PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  # Only the IADE_API marked interface of facade/public leaves the shared
  # object. The ImagesAnnotatorDataDrivers library linked below is built from
  # the very same project template and exports its own default_logger and
  # project_decls symbols: exporting ours too would let the dynamic linker bind
  # one library's calls to the other library's definitions. It does not cover
  # the factory - the std::make_shared instantiations stay weak and exported
  # whatever the visibility - which is why our implementation namespace is
  # iade0impl instead of the template's lib0impl.
  CXX_VISIBILITY_PRESET hidden
)

target_compile_definitions(
  ${IADE_LIBRARY_NAME}
  PRIVATE IMAGES_ANNOTATOR_DATA_EXPORTERS_BUILDING=1
)

add_subdirectory(facade)
add_subdirectory(libmain)
add_subdirectory(tests)

include(lib-cmake-module-gen)
