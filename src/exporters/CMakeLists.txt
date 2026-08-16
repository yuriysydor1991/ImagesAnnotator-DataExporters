cmake_minimum_required(VERSION 3.13)

# One sub-directory per dataset layout, each with its own sources and its own
# unit tests. ExportersAliases.h stays here, since every one of them shares it.

add_subdirectory(PlainTxt)
add_subdirectory(Yolo4)
add_subdirectory(Ultralytics)
add_subdirectory(Coco)
add_subdirectory(PascalVoc)
add_subdirectory(PyTorch)

add_subdirectory(tests)
