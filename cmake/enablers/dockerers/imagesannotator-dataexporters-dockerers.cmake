cmake_minimum_required(VERSION 3.13)

option(
  IADE_ENABLE_DOCKER
  "Set to ON to enable the dockers run"
  OFF
)

if (NOT IADE_ENABLE_DOCKER)
    return()
endif()

message(STATUS "Docker runner target enabled")

find_program(DOCKER_EXEC docker REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/dockerers")

include(imagesannotator-dataexporters-docker-variables-declare)
include(imagesannotator-dataexporters-docker-Jenkins-pipeline-runner-target)
