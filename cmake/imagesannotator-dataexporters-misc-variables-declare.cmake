cmake_minimum_required(VERSION 3.13)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ON by default: the public API namespace carries the major and the minor
# version (ImagesAnnotatorDataExporters015), so the installable names track the
# same pair. Two minor versions then install completely side by side - their
# binaries, their header directories and their CMake packages all differ. This
# is also what the ImagesAnnotatorDataDrivers library this project reads its
# records through does, so both halves of the pair are named alike.
option(
  IADE_LIB_INCLUDE_MINOR_IN_NAME
  "Append .<minor> to the installable library name (binary, headers subdir, CMake package dir)"
  ON
)

option(
  IADE_LIB_INCLUDE_MICRO_IN_NAME
  "Append .<micro> after the minor segment to the installable library name. Implies IADE_LIB_INCLUDE_MINOR_IN_NAME."
  OFF
)

set(
  IADE_LIB_NAME_SUFFIX ""
  CACHE STRING
  "Optional trailing suffix appended to the installable library name (e.g. '-dev')."
)

set(_lib_name "${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}")

if(IADE_LIB_INCLUDE_MICRO_IN_NAME AND NOT IADE_LIB_INCLUDE_MINOR_IN_NAME)
  message(WARNING
    "IADE_LIB_INCLUDE_MICRO_IN_NAME=ON requires IADE_LIB_INCLUDE_MINOR_IN_NAME=ON - "
    "enabling IADE_LIB_INCLUDE_MINOR_IN_NAME implicitly."
  )
  set(IADE_LIB_INCLUDE_MINOR_IN_NAME ON CACHE BOOL "" FORCE)
endif()

if(IADE_LIB_INCLUDE_MINOR_IN_NAME)
  set(_lib_name "${_lib_name}.${PROJECT_VERSION_MINOR}")
endif()

if(IADE_LIB_INCLUDE_MICRO_IN_NAME)
  set(_lib_name "${_lib_name}.${PROJECT_VERSION_PATCH}")
endif()

if(IADE_LIB_NAME_SUFFIX)
  set(_lib_name "${_lib_name}${IADE_LIB_NAME_SUFFIX}")
endif()

set(
  IADE_LIBRARY_NAME "${_lib_name}"
  CACHE STRING "Project main library name and target"
  FORCE
)

unset(_lib_name)

message(STATUS "IADE_LIBRARY_NAME: ${IADE_LIBRARY_NAME}")

set(
  IADE_MAINTAINER "Your Name"
  CACHE STRING "Project maintainer name (used for the DEB package)"
)

set(
  IADE_MAINTAINER_EMAIL "yourname@your.email.org"
  CACHE STRING "Project maintainer email (used for the DEB package)"
)

set(
  IADE_MAX_LOG_LEVEL "3"
  CACHE STRING
  "Sets the maximum severity of the compiled log messages. Error=0. Trace=5. By default Info=3"
)

set(
  IADE_DEFAULT_LOG_FILE_PATH ""
  CACHE STRING
  "Sets the default log file path"
)

option(
  IADE_ENABLE_LOGS_MICROSECONDS_TIME
  "Set to OFF to disable the microseconds fields included into the log messages"
  ON
)

string(TIMESTAMP PROJECT_CONFIGURE_DATE "%Y-%m-%d %H:%M:%S")
