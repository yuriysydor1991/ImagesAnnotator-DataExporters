cmake_minimum_required(VERSION 3.13)

if(NOT IADE_ENABLE_COMPONENT_TESTS)
  return()
endif()

add_subdirectory(Exporters)
