cmake_minimum_required(VERSION 3.13)

add_executable(
  UTEST_LibFactory
  UTEST_LibFactory.cpp
  ${CMAKE_SOURCE_DIR}/src/lib/libmain/LibFactory.cpp
  ${CMAKE_SOURCE_DIR}/src/lib/libmain/LibMain.cpp
  ${IADE_EXPORTERS_SRC}
  ${IADE_HELPERS_SRC}
  ${IADE_CURL_SRC}
)

target_include_directories(
  UTEST_LibFactory
  PRIVATE ${GENERAL_MOCKS_ROOT}/log
  PRIVATE ${GENERAL_MOCKS_ROOT}/project-global-decls
  PRIVATE ${CMAKE_SOURCE_DIR}
  PRIVATE ${IADE_PUBLIC_INCLUDE_DIR}
)

target_link_libraries(
  UTEST_LibFactory
  GTest::gtest_main GTest::gmock
  ${IMAGES_ANNOTATOR_DATA_DRIVERS_TARGET} CURL::libcurl
)

include(GoogleTest)

gtest_add_tests(
  TARGET UTEST_LibFactory
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
