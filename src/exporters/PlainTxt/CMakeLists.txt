cmake_minimum_required(VERSION 3.13)

# the plain text dataset layout exporter

target_sources(
  ${PROJECT_LIBRARY_NAME}
  PRIVATE
    PlainTxt2FolderExporter.cpp
)

add_subdirectory(tests)
