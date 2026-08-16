cmake_minimum_required(VERSION 3.13)

# the YOLO v4 dataset layout exporter and the network
# configuration writer it hands its cfg/yolov4-obj.cfg stream to

target_sources(
  ${PROJECT_LIBRARY_NAME}
  PRIVATE
    Yolo42FolderExporter.cpp
    Yolov4CfgWriter.cpp
)

add_subdirectory(tests)
