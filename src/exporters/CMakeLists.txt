cmake_minimum_required(VERSION 3.13)

target_sources(
  ${PROJECT_LIBRARY_NAME}
  PRIVATE
    PlainTxt2FolderExporter.cpp
    Yolo42FolderExporter.cpp
    Yolov4CfgWriter.cpp
    PyTorchVisionFolderExporter.cpp
)

add_subdirectory(tests)
