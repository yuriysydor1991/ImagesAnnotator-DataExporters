cmake_minimum_required(VERSION 3.13)

option(
  IADE_ENABLE_WIX
  "Set to ON to enable the WIX MSI package creation from the project build artifacts (MS Windows)"
  OFF
)

if(NOT IADE_ENABLE_WIX)
    return()
endif()

include(InstallRequiredSystemLibraries)

set(
  IADE_WIX_UPGRADE_GUID
  "12345678-1234-1234-1234-123456789012"
  CACHE STRING
  "Stable GUID identifying the product across upgrades - MUST be replaced with a project-unique value for a real release"
)

set(
  IADE_WIX_PROGRAM_MENU_FOLDER
  "${PROJECT_NAME}"
  CACHE STRING
  "Windows Start Menu folder name where the installer creates shortcuts"
)

set(
  IADE_WIX_CULTURES
  "en-US"
  CACHE STRING
  "Semicolon-separated list of UI cultures used by the WIX installer (e.g. en-US;uk-UA)"
)

set(
  IADE_WIX_LICENSE_RTF
  ""
  CACHE STRING
  "Optional absolute path to a RTF or TXT license file shown by the MSI installer"
)

set(
  IADE_WIX_PRODUCT_ICON
  ""
  CACHE STRING
  "Optional absolute path to an .ico file used as the MSI product icon"
)

# the architecture the CPack WIX generator picks when given none
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(wixArchitecture x64)
else()
  set(wixArchitecture x86)
endif()

set(
  IADE_WIX_ARCHITECTURE
  ${wixArchitecture}
  CACHE STRING
  "The destination CPU architecture of the MSI installer (e.g. x64, arm64, x86)"
)

set(CPACK_GENERATOR "WIX")
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT ${IADE_MAINTAINER_EMAIL})
set(CPACK_PACKAGE_VENDOR "${IADE_MAINTAINER}")
set(CPACK_PACKAGE_DESCRIPTION ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})

set(CPACK_WIX_UPGRADE_GUID ${IADE_WIX_UPGRADE_GUID})
set(CPACK_WIX_PROGRAM_MENU_FOLDER ${IADE_WIX_PROGRAM_MENU_FOLDER})
set(CPACK_WIX_CULTURES ${IADE_WIX_CULTURES})
set(CPACK_WIX_ROOT_FOLDER_ID "ProgramFiles64Folder")
set(CPACK_WIX_ARCHITECTURE ${IADE_WIX_ARCHITECTURE})

if(IADE_WIX_LICENSE_RTF)
  set(CPACK_WIX_LICENSE_RTF ${IADE_WIX_LICENSE_RTF})
endif()

if(IADE_WIX_PRODUCT_ICON)
  set(CPACK_WIX_PRODUCT_ICON ${IADE_WIX_PRODUCT_ICON})
endif()

# the CPACK_SYSTEM_NAME of the default file name (win64 or win32) names no CPU,
# so the destination one takes its place, followed by the name and the version
# of the destination system
include(imagesannotator-dataexporters-package-system)

set(
  CPACK_PACKAGE_FILE_NAME
  ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${IADE_WIX_ARCHITECTURE}${IADE_PACKAGE_SYSTEM_SUFFIX}
)

include(CPack)
