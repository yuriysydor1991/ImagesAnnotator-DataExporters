cmake_minimum_required(VERSION 3.13)

add_executable(
  UTEST_Yolov4CfgWriter
  UTEST_Yolov4CfgWriter.cpp
  ${CMAKE_SOURCE_DIR}/src/exporters/Yolov4CfgWriter.cpp
)

target_include_directories(
  UTEST_Yolov4CfgWriter
  PRIVATE ${CMAKE_SOURCE_DIR}
)

target_link_libraries(
  UTEST_Yolov4CfgWriter
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

gtest_add_tests(
  TARGET UTEST_Yolov4CfgWriter
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
