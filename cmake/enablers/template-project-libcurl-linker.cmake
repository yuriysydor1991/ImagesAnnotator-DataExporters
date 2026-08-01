cmake_minimum_required(VERSION 3.13)

# Companion of the template-project-libcurl-enabler module: links the already
# made available libcurl against the project library target, which only exists
# once src/ has been added.

if (NOT ENABLE_LIBCURL)
  return()
endif()

# PRIVATE on purpose: no installable public header of the library exposes a
# libcurl type, so the dependency must not leak into the exported interface.
target_link_libraries(
  ${PROJECT_LIBRARY_NAME}
  PRIVATE CURL::libcurl
)
