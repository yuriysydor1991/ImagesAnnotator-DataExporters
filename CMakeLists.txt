cmake_minimum_required(VERSION 3.13)

project(
  ImagesAnnotatorDataExporters
  VERSION 0.15.1
  DESCRIPTION "The ImagesAnnotator annotations dataset exporters library"
  HOMEPAGE_URL "http://www.kytok.org.ua/"
  LANGUAGES CXX
)

# Taken in by another project (FetchContent or add_subdirectory) the library
# builds alone: none of its tests, tools, packages and documentation.
string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}" IADE_IS_TOP_LEVEL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/imagesannotator-dataexporters-bootstrap.cmake)

message(STATUS "PROJECT_NAME: ImagesAnnotatorDataExporters")

add_subdirectory(src)

if(IADE_IS_TOP_LEVEL)
  add_subdirectory(doc)
endif()

include(imagesannotator-dataexporters-feature-enabler-post)
