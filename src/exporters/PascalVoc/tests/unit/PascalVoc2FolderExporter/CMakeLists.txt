cmake_minimum_required(VERSION 3.13)

add_executable(
  UTEST_PascalVoc2FolderExporter
  UTEST_PascalVoc2FolderExporter.cpp
  ${IADE_PASCAL_VOC_SRC}
  ${IADE_FACADE_SRC}
  ${IADE_HELPERS_SRC}
  ${IADE_CURL_SRC}
)

target_include_directories(
  UTEST_PascalVoc2FolderExporter
  PRIVATE ${GENERAL_MOCKS_ROOT}/log
  PRIVATE ${GENERAL_MOCKS_ROOT}/project-global-decls
  PRIVATE ${CMAKE_SOURCE_DIR}
  PRIVATE ${IADE_PUBLIC_INCLUDE_DIR}
)

target_link_libraries(
  UTEST_PascalVoc2FolderExporter
  GTest::gtest_main GTest::gmock
  ${IMAGES_ANNOTATOR_DATA_DRIVERS_TARGET} CURL::libcurl
)

include(GoogleTest)

gtest_add_tests(
  TARGET UTEST_PascalVoc2FolderExporter
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
