cmake_minimum_required(VERSION 3.13)

if (IADE_ENABLE_LOGS_MICROSECONDS_TIME)
  add_compile_definitions(ENABLE_LOGS_MICROSECONDS_TIME=1)
endif()

add_subdirectory(default-logger)
