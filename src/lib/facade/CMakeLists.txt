cmake_minimum_required(VERSION 3.13)

target_sources(
  ${IADE_LIBRARY_NAME}
  PRIVATE
    IADataExportersFacade.cpp
    ${IADE_FACADE_SRC}
)

install(
  DIRECTORY public/
  DESTINATION include/${IADE_LIBRARY_NAME}
  COMPONENT Development
)

add_subdirectory(tests)
