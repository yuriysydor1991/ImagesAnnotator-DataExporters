cmake_minimum_required(VERSION 3.13)

option(
  IADE_ENABLE_SANITIZERS
  "Set to ON to enable the sanitizers compile options"
  OFF
)

option(
  IADE_ENABLE_SANITIZERS_THREADS
  "Set to ON to enable the sanitizers for the threads compile options"
  OFF
)

if (NOT (IADE_ENABLE_SANITIZERS OR IADE_ENABLE_SANITIZERS_THREADS))
  return()
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/sanitizers")

if (IADE_ENABLE_SANITIZERS)
  include(imagesannotator-dataexporters-sanitizers-leak-and-address)
endif()

if (IADE_ENABLE_SANITIZERS_THREADS)
  include(imagesannotator-dataexporters-sanitizers-threads)
endif()
