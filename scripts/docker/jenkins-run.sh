#!/bin/bash -e

PROJECT_ROOT=$(realpath "$(dirname "$0")/../..")
BUILD_SCRIPTS_ROOT=$(realpath "${PROJECT_ROOT}/scripts/build")

${BUILD_SCRIPTS_ROOT}/release-configure.sh \
  -DIADE_ENABLE_DOCKER=ON \
  -DIADE_ENABLE_JENKINS_DOCKER_PIPELINE=ON \
  "$@"

${BUILD_SCRIPTS_ROOT}/release-build.sh --target jenkins-pipeline-docker-run "$@"
