cmake_minimum_required(VERSION 3.13)

# the Create ML object detection dataset layout exporter

target_sources(
  ${PROJECT_LIBRARY_NAME}
  PRIVATE
    CreateML2FolderExporter.cpp
)

add_subdirectory(tests)
