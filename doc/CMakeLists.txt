cmake_minimum_required(VERSION 3.13)

option(
  IADE_ENABLE_DOC_DOXYGEN 
  "Set to ON value if Doxygen documentation generation should be available"
  OFF
)

set(
  IADE_DOXYGEN_OUTPUT_LANGUAGE "English"
  CACHE STRING "Sets the Doxygen output language for the generated documentation (see doc/Doxyfile.in)"
)

set(
  IADE_DOXYGEN_MAIN_PAGE_MD "README.md"
  CACHE STRING "Sets the Doxygen main README file to place into documentation the home page."
)

set(
  IADE_DOXYGEN_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/doc
  CACHE STRING "Sets the doxygen documentation output directory."
)

option(
  IADE_DOXYGEN_DO_INSTALL
  "Set to on if CMake should install the generated documentation during install command executed."
  OFF 
)

set(
  IADE_DOXYGEN_OUT_HTML_NAME ${IADE_LIBRARY_NAME}-html
  CACHE STRING "The Doxygen output HTML directory name (for the Doxyfile)"
)

set(
  IADE_DOXYGEN_OUT_HTML ${IADE_DOXYGEN_OUTPUT_DIR}/${IADE_DOXYGEN_OUT_HTML_NAME}
  CACHE STRING "Sets the doxygen documentation output full directory path."
)

if(NOT IADE_ENABLE_DOC_DOXYGEN)
  return()
endif()

find_package(Doxygen REQUIRED)

set(DOXYGEN_IN Doxyfile.in)
set(DOXYGEN_OUT ${PROJECT_BINARY_DIR}/Doxyfile)

if (IADE_DOXYGEN_OUTPUT_LANGUAGE STREQUAL "Ukrainian")
  set(IADE_DOXYGEN_MAIN_PAGE_MD "doc/README.uk_UA.md")
endif()

configure_file(
  ${DOXYGEN_IN} 
  ${DOXYGEN_OUT}
)

add_custom_target(
  Doxygen-doc
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMENT "Generating project documentation with Doxygen"
  VERBATIM
)

if(IADE_DOXYGEN_DO_INSTALL)
  message(STATUS "Doxygen html doc install enabled for the ${IADE_DOXYGEN_OUT_HTML} directory")

  install(
    DIRECTORY ${IADE_DOXYGEN_OUT_HTML}
    DESTINATION share/docs
    COMPONENT Documentation
  )
endif()
