cmake_minimum_required(VERSION 3.13)

if(NOT ENABLE_UNIT_TESTS)
  return()
endif()

# disabling the asserts for the tests
add_compile_options(-DNDEBUG=1)

add_subdirectory(Dxxwjz2FolderExporter)
add_subdirectory(Dxxwjz2NetworkWriter)
add_subdirectory(Dxxwjz3CfgJson)
add_subdirectory(Dxxwjz3NetworkWriter)
add_subdirectory(Dxxwjz3FolderExporter)
add_subdirectory(Dxxwjz2pAnnotationsWriter)
add_subdirectory(Dxxwjz2pProjectWriter)
add_subdirectory(Dxxwjz3pProjectWriter)
add_subdirectory(Dxxwjz2pProjectExporter)
add_subdirectory(Dxxwjz3pProjectExporter)
