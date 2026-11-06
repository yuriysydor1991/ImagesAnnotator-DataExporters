cmake_minimum_required(VERSION 3.13)

if(NOT IADE_ENABLE_UNIT_TESTS)
  return()
endif()

add_subdirectory(unit)
