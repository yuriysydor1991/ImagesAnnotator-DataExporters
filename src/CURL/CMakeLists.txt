cmake_minimum_required(VERSION 3.13)

# The libcurl is made available (system wide or through the Internet) by the
# template-project-libcurl-enabler module and linked against the library by
# its template-project-libcurl-linker companion.

target_sources(
  ${PROJECT_LIBRARY_NAME}
  PRIVATE
    CURLController.cpp
)

add_subdirectory(tests)
