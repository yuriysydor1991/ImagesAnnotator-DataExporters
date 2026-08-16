cmake_minimum_required(VERSION 3.13)

# the PyTorch Vision dataset layout exporter

target_sources(
  ${PROJECT_LIBRARY_NAME}
  PRIVATE
    PyTorchVisionFolderExporter.cpp
)

add_subdirectory(tests)
