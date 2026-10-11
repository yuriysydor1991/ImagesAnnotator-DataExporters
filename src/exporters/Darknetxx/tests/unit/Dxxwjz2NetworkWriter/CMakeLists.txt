cmake_minimum_required(VERSION 3.13)

add_executable(
  UTEST_Dxxwjz2NetworkWriter
  UTEST_Dxxwjz2NetworkWriter.cpp
  ${IADE_EXPORTERS_SRC_DIR}/Darknetxx/Dxxwjz2NetworkWriter.cpp
)

target_include_directories(
  UTEST_Dxxwjz2NetworkWriter
  PRIVATE ${GENERAL_MOCKS_ROOT}/project-global-decls
  PRIVATE ${CMAKE_SOURCE_DIR}
)

target_link_libraries(
  UTEST_Dxxwjz2NetworkWriter
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

gtest_add_tests(
  TARGET UTEST_Dxxwjz2NetworkWriter
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
