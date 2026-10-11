cmake_minimum_required(VERSION 3.13)

add_executable(
  UTEST_Dxxwjz3NetworkWriter
  UTEST_Dxxwjz3NetworkWriter.cpp
  ${IADE_EXPORTERS_SRC_DIR}/Darknetxx/Dxxwjz3NetworkWriter.cpp
  ${IADE_EXPORTERS_SRC_DIR}/Darknetxx/Dxxwjz3CfgJson.cpp
  ${IADE_EXPORTERS_SRC_DIR}/Darknetxx/Dxxwjz2NetworkWriter.cpp
)

target_include_directories(
  UTEST_Dxxwjz3NetworkWriter
  PRIVATE ${GENERAL_MOCKS_ROOT}/project-global-decls
  PRIVATE ${CMAKE_SOURCE_DIR}
)

target_link_libraries(
  UTEST_Dxxwjz3NetworkWriter
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

gtest_add_tests(
  TARGET UTEST_Dxxwjz3NetworkWriter
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
