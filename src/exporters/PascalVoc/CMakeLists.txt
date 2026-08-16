cmake_minimum_required(VERSION 3.13)

# the Pascal VOC dataset layout exporter

target_sources(
  ${PROJECT_LIBRARY_NAME}
  PRIVATE
    PascalVoc2FolderExporter.cpp
)

add_subdirectory(tests)
