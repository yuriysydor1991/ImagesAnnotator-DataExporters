cmake_minimum_required(VERSION 3.13)

# the COCO object detection dataset layout exporter

target_sources(
  ${PROJECT_LIBRARY_NAME}
  PRIVATE
    Coco2FolderExporter.cpp
)

add_subdirectory(tests)
