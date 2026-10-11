cmake_minimum_required(VERSION 3.13)

add_executable(
  UTEST_Dxxwjz3pProjectExporter
  UTEST_Dxxwjz3pProjectExporter.cpp
  ${IADE_DARKNETXX_SRC}
  ${IADE_YOLO4_SRC}
  ${IADE_FACADE_SRC}
  ${IADE_HELPERS_SRC}
  ${IADE_CURL_SRC}
)

target_include_directories(
  UTEST_Dxxwjz3pProjectExporter
  PRIVATE ${GENERAL_MOCKS_ROOT}/log
  PRIVATE ${GENERAL_MOCKS_ROOT}/project-global-decls
  PRIVATE ${CMAKE_SOURCE_DIR}
  PRIVATE ${IADE_PUBLIC_INCLUDE_DIR}
)

target_link_libraries(
  UTEST_Dxxwjz3pProjectExporter
  GTest::gtest_main GTest::gmock ZLIB::ZLIB
  ${IMAGES_ANNOTATOR_DATA_DRIVERS_TARGET} CURL::libcurl
)

include(GoogleTest)

gtest_add_tests(
  TARGET UTEST_Dxxwjz3pProjectExporter
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
