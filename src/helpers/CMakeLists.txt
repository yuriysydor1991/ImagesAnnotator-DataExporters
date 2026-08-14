cmake_minimum_required(VERSION 3.13)

target_sources(
  ${PROJECT_LIBRARY_NAME}
  PRIVATE
    ImageRecordUrlAndPathHelper.cpp
    ImageLoader.cpp
)

add_subdirectory(tests)
