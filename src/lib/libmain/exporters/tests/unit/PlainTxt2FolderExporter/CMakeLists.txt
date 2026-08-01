cmake_minimum_required(VERSION 3.13)

add_executable(
  UTEST_PlainTxt2FolderExporter
  UTEST_PlainTxt2FolderExporter.cpp
  ${CMAKE_SOURCE_DIR}/src/lib/libmain/exporters/PlainTxt2FolderExporter.cpp
  ${IADE_HELPERS_SRC}
  ${IADE_CURL_SRC}
)

target_include_directories(
  UTEST_PlainTxt2FolderExporter
  PRIVATE ${GENERAL_MOCKS_ROOT}/log
  PRIVATE ${GENERAL_MOCKS_ROOT}/project-global-decls
  PRIVATE ${CMAKE_SOURCE_DIR}
  PRIVATE ${IADE_PUBLIC_INCLUDE_DIR}
)

target_link_libraries(
  UTEST_PlainTxt2FolderExporter
  GTest::gtest_main GTest::gmock
  ${IMAGES_ANNOTATOR_DATA_DRIVERS_TARGET} CURL::libcurl
)

include(GoogleTest)

gtest_add_tests(
  TARGET UTEST_PlainTxt2FolderExporter
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
