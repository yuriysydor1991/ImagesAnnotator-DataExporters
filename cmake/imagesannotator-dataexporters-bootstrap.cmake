cmake_minimum_required(VERSION 3.13)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# The tests switches are read all over the src subtree, so plain OFF variables
# hide whatever the cache of the including project holds.
if(NOT IADE_IS_TOP_LEVEL)
  set(IADE_ENABLE_UNIT_TESTS OFF)
  set(IADE_ENABLE_COMPONENT_TESTS OFF)

  message(STATUS "${PROJECT_NAME} is a dependency of the ${CMAKE_PROJECT_NAME}: building the library alone")
endif()

include(imagesannotator-dataexporters-misc-variables-declare)
include(imagesannotator-dataexporters-feature-enabler)
