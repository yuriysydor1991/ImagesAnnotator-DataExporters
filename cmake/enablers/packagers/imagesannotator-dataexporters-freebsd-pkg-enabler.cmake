cmake_minimum_required(VERSION 3.13)

option(
  IADE_ENABLE_FREEBSD_PKG
  "Set to ON to enable the FreeBSD pkg package creation from the project build artifacts"
  OFF
)

if(NOT IADE_ENABLE_FREEBSD_PKG)
    return()
endif()

include(InstallRequiredSystemLibraries)

set(
  IADE_FREEBSD_PKG_LICENSE
  "BSD2CLAUSE"
  CACHE STRING
  "FreeBSD pkg license tag (e.g. BSD2CLAUSE, BSD3CLAUSE, GPLv3, MIT)"
)

# IADE_LIBRARY_NAME_lower is computed here on purpose: the docker enabler
# that also declares it returns early when IADE_ENABLE_DOCKER is off, so relying on
# it would leave the origin without its name part.
string(TOLOWER ${IADE_LIBRARY_NAME} IADE_LIBRARY_NAME_lower)

set(
  IADE_FREEBSD_PKG_ORIGIN
  "misc/${IADE_LIBRARY_NAME_lower}"
  CACHE STRING
  "FreeBSD pkg origin in <category>/<name> form"
)

set(
  IADE_FREEBSD_PKG_CATEGORIES
  "misc"
  CACHE STRING
  "Semicolon-separated list of FreeBSD pkg categories"
)

set(CPACK_GENERATOR "FREEBSD")
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT ${IADE_MAINTAINER_EMAIL})
set(CPACK_PACKAGE_VENDOR "${IADE_MAINTAINER}")
set(CPACK_PACKAGE_DESCRIPTION ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})

set(CPACK_FREEBSD_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_FREEBSD_PACKAGE_COMMENT ${PROJECT_DESCRIPTION})
set(CPACK_FREEBSD_PACKAGE_DESCRIPTION ${PROJECT_DESCRIPTION})
set(CPACK_FREEBSD_PACKAGE_WWW ${PROJECT_HOMEPAGE_URL})
set(CPACK_FREEBSD_PACKAGE_LICENSE ${IADE_FREEBSD_PKG_LICENSE})
set(CPACK_FREEBSD_PACKAGE_MAINTAINER "${IADE_MAINTAINER_EMAIL}")
set(CPACK_FREEBSD_PACKAGE_ORIGIN ${IADE_FREEBSD_PKG_ORIGIN})
set(CPACK_FREEBSD_PACKAGE_CATEGORIES ${IADE_FREEBSD_PKG_CATEGORIES})
set(CPACK_FREEBSD_PACKAGE_DEPS "")

# the CPACK_SYSTEM_NAME of the default file name names the system alone, so the
# destination CPU takes its place (the uname -p one, as in the package ABI),
# followed by the name and the version of the destination system
include(imagesannotator-dataexporters-package-system)

set(
  CPACK_PACKAGE_FILE_NAME
  ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_PROCESSOR}${IADE_PACKAGE_SYSTEM_SUFFIX}
)

include(CPack)
