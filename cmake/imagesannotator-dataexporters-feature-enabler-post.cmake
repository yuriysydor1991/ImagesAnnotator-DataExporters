cmake_minimum_required(VERSION 3.13)

if(IADE_IS_TOP_LEVEL)
  include(imagesannotator-dataexporters-dockerers)
endif()

# The mandatory dependencies were made available by the pre-src enabler pass,
# here they only get linked against the now existing library target.
include(imagesannotator-dataexporters-data-drivers-linker)
include(imagesannotator-dataexporters-libcurl-linker)
include(imagesannotator-dataexporters-zlib-linker)
include(imagesannotator-dataexporters-OpenCV-linker)

