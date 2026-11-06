cmake_minimum_required(VERSION 3.13)

# The libcurl is made available (system wide or through the Internet) by the
# imagesannotator-dataexporters-libcurl-enabler module and linked against the library by
# its imagesannotator-dataexporters-libcurl-linker companion.

target_sources(
  ${IADE_LIBRARY_NAME}
  PRIVATE
    CURLController.cpp
)

add_subdirectory(tests)
