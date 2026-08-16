cmake_minimum_required(VERSION 3.13)

# the Ultralytics YOLO dataset layout: the directory and descriptor writer
# every task shares, and the label file line of each task written on top of it

target_sources(
  ${PROJECT_LIBRARY_NAME}
  PRIVATE
    Ultralytics2FolderExporter.cpp
    UltralyticsDetect2FolderExporter.cpp
    UltralyticsObb2FolderExporter.cpp
)

add_subdirectory(tests)
