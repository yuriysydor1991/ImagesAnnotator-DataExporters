cmake_minimum_required(VERSION 3.13)

option(
  IADE_COMPILE_WARNINGS_AS_ERRORS
  "Set to ON if to pass -Werror compile parameter to the GCC compiler."
  OFF
)

option(
  IADE_ENABLE_COMPILER_CODE_ANALYZER
  "Set to ON if to pass -fanalyzer compile parameter to the GCC compiler. May dramatically increase the compile time."
  OFF
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/compile-options")

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  include(imagesannotator-dataexporters-compile-options-MSVC)
else()
  include(imagesannotator-dataexporters-compile-options-GCC)
endif()

set(
  IADE_EXTRA_COMPILE_OPTIONS 
  ${IADE_EXTRA_COMPILE_OPTIONS} 
  -DMAX_LOG_LEVEL=${IADE_MAX_LOG_LEVEL}
  -DDEFAULT_LOG_FILE_PATH="${IADE_DEFAULT_LOG_FILE_PATH}"
)

message(STATUS "COMPILER ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "COMPILE OPTIONS: ${IADE_EXTRA_COMPILE_OPTIONS}")
