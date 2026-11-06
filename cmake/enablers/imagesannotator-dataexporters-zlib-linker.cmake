cmake_minimum_required(VERSION 3.13)

# Companion of the imagesannotator-dataexporters-zlib-enabler module: links the already
# made available zlib against the project library target, which only exists
# once src/ has been added.

# PRIVATE on purpose: no installable public header of the library exposes a
# zlib type, so the dependency must not leak into the exported interface.
target_link_libraries(
  ${IADE_LIBRARY_NAME}
  PRIVATE ZLIB::ZLIB
)
