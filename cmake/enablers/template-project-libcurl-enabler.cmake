cmake_minimum_required(VERSION 3.13)

# The image loader preloads the web hosted image records through libcurl, so
# unlike in the original template this dependency is mandatory for the library
# to build.
#
# This module only makes the dependency AVAILABLE. It runs before the library
# target exists, so that the unit and component test executables declared
# inside src/ may already link against it. The linking against the library
# target itself is performed later by the companion
# template-project-libcurl-linker module.
option(
  ENABLE_LIBCURL
  "Set to ON to enable the libcurl (by using system wide available or through the Internet). Required by the image loader."
  ON
)

if (NOT ENABLE_LIBCURL)
  return()
endif()

set(TEMPLATE_APP_CURL_GIT "https://github.com/curl/curl.git" CACHE STRING "The CURL library git source repository")
set(TEMPLATE_APP_CURL_GIT_TAG "master" CACHE STRING "The CURL project git repository tag of interest")

template_project_default_3rdparty_enabler(
  NAME CURL
  GIT_REPOSITORY ${TEMPLATE_APP_CURL_GIT}
  GIT_TAG ${TEMPLATE_APP_CURL_GIT_TAG}
)
