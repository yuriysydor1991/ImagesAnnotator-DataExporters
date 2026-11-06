cmake_minimum_required(VERSION 3.13)

# The darknetxx dxxwjz files the exporters write are the gzip compressed JSON
# documents, which the zlib compresses and decompresses (see the
# ZlibController of src/zlib, the one of the cpp-app-template appZlib branch),
# so unlike in the original template this dependency is mandatory for the
# library to build.
#
# This module only makes the dependency AVAILABLE. It runs before the library
# target exists, so that the unit and component test executables declared
# inside src/ may already link against it. The linking against the library
# target itself is performed later by the companion
# imagesannotator-dataexporters-zlib-linker module.
option(
  IADE_ENABLE_ZLIB
  "Set to ON to enable the zlib compression library (by using system wide available or through the Internet). Required by the darknetxx dxxwjz files."
  ON
)

if (NOT IADE_ENABLE_ZLIB)
  message(FATAL_ERROR "The zlib is mandatory: the darknetxx dxxwjz files are gzip compressed")
endif()

set(IADE_ZLIB_GIT "https://github.com/madler/zlib.git" CACHE STRING "The zlib library git source repository")
set(IADE_ZLIB_GIT_TAG "v1.3.1" CACHE STRING "The zlib project git repository tag of interest")

# the fetched zlib is linked statically, so its examples are of no use
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "Build the zlib examples" FORCE)

iade_default_3rdparty_enabler(
  NAME ZLIB
  GIT_REPOSITORY ${IADE_ZLIB_GIT}
  GIT_TAG ${IADE_ZLIB_GIT_TAG}
)

# The system zlib comes as the ZLIB::ZLIB target, while the fetched one
# declares the zlibstatic (static) and the zlib (shared) targets. The alias
# gives the project the single ZLIB::ZLIB target to link for both of them.
if (NOT TARGET ZLIB::ZLIB AND TARGET zlibstatic)
  set_target_properties(zlibstatic PROPERTIES POSITION_INDEPENDENT_CODE ON)
  add_library(ZLIB::ZLIB ALIAS zlibstatic)
endif()

if (NOT TARGET ZLIB::ZLIB)
  message(FATAL_ERROR "The zlib library made available provides no ZLIB::ZLIB target")
endif()
