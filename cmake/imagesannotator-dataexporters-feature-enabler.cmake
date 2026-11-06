cmake_minimum_required(VERSION 3.13)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/tools")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/enablers")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/compile-options")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/compile-options/sanitizers")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/enablers/dockerers")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/enablers/packagers")

include(imagesannotator-dataexporters-enabler-function)
include(imagesannotator-dataexporters-git-enabler)

# Mandatory library dependencies. They are made available here, before src/ is
# added, so that both the library target and the test executables declared
# inside it can be linked against them.
include(imagesannotator-dataexporters-data-drivers-enabler)
include(imagesannotator-dataexporters-libcurl-enabler)
include(imagesannotator-dataexporters-zlib-enabler)

# Optional: when OpenCV is there the library also ships an image cropper of its
# own, so a consumer with no imaging stack of its own still gets the PyTorch
# Vision export. A missing OpenCV only drops that cropper.
include(imagesannotator-dataexporters-OpenCV-enabler)

# The tests, the tools and the packages of the top level build only: the same
# named targets and the CPack of an including project stay its own.
if(IADE_IS_TOP_LEVEL)
  include(imagesannotator-dataexporters-GTest-enabler)
  include(imagesannotator-dataexporters-clang-format-target)

  # packagers
  include(imagesannotator-dataexporters-deb-enabler)
  include(imagesannotator-dataexporters-freebsd-pkg-enabler)
  include(imagesannotator-dataexporters-wix-enabler)
  include(imagesannotator-dataexporters-rpm-enabler)

  # compile options
  include(imagesannotator-dataexporters-sanitizers)

  # analyzers
  include(imagesannotator-dataexporters-cppcheck-target)
  include(imagesannotator-dataexporters-clang-tidy-target)
endif()
