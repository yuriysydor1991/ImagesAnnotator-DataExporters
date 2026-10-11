cmake_minimum_required(VERSION 3.13)

# the darknetxx layouts exporters: the training directories, the YOLO v4 one
# with the dxxwjz2 or the dxxwjz3 network to train, and the dxxwjz2p and the
# dxxwjz3p training projects, with the writers of their documents

target_sources(
  ${PROJECT_LIBRARY_NAME}
  PRIVATE
    Dxxwjz2FolderExporter.cpp
    Dxxwjz2NetworkWriter.cpp
    Dxxwjz3CfgJson.cpp
    Dxxwjz3NetworkWriter.cpp
    Dxxwjz3FolderExporter.cpp
    Dxxwjz2pAnnotationsWriter.cpp
    Dxxwjz2pProjectWriter.cpp
    Dxxwjz3pProjectWriter.cpp
    Dxxwjz2pProjectExporter.cpp
    Dxxwjz3pProjectExporter.cpp
)

add_subdirectory(tests)
