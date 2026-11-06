cmake_minimum_required(VERSION 3.13)

string(TOLOWER ${IADE_LIBRARY_NAME} IADE_LIBRARY_NAME_lower)

set(
  IADE_DOCKER_HOST_ADDRESS
  "127.0.0.1"
  CACHE STRING "The docker service machine address"
)

set(
  IADE_DOCKER_HOST_PORT
  "2375"
  CACHE STRING "The docker service machine port"
)

set(
  IADE_DOCKER_HOST_STR
  "tcp://${IADE_DOCKER_HOST_ADDRESS}:${IADE_DOCKER_HOST_PORT}"
  CACHE STRING "The docker service machine full address"
)

set(
  IADE_DOCKERFILE_SINGLE_RUN_BASE_IMAGE_NAME
  "jenkins/jenkins"
  CACHE STRING 
  "The Dockerfile source environment name (FROM ubuntu, debian, jenkins etc.)"
)

set(
  IADE_DOCKERFILE_SINGLE_RUN_BASE_IMAGE_VERSION
  "lts"
  CACHE STRING 
  "The Dockerfile source environment version"
)

set(
  IADE_DOCKERFILE_SINGLE_RUN_BASE_IMAGE_STRING
  "${IADE_DOCKERFILE_SINGLE_RUN_BASE_IMAGE_NAME}:${IADE_DOCKERFILE_SINGLE_RUN_BASE_IMAGE_VERSION}"
  CACHE STRING 
  "The Dockerfile source environment full string (e.g. ubuntu:22.04)"
)

set(
  PROJECT_BINARY_PATH
  ${PROJECT_BINARY_DIR}/src/${IADE_LIBRARY_NAME}
)

message(STATUS "Docker executable: ${DOCKER_EXEC}")
message(STATUS "Dockerfile configure source full path: ${DOCKERFILE_SRC}")
message(STATUS "Dockerfile destination name: ${DOCKERFILE_DST}")
message(STATUS "Docker host address and port: ${IADE_DOCKER_HOST_STR}")
message(STATUS "Project current branch name: ${PROJECT_CURRENT_BRANCH_NAME}")
message(STATUS "Project binary filepath: ${PROJECT_BINARY_PATH}")
message(STATUS "Docker source environment base image string: ${IADE_DOCKERFILE_SINGLE_RUN_BASE_IMAGE_STRING}")
