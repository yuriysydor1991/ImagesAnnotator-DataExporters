cmake_minimum_required(VERSION 3.13)

# Unlike the unit tests, this one compiles nothing of the library: it links the
# produced shared object and drives it through the installable public headers
# only, exactly the way a downstream project does.
add_executable(
  CTEST_Exporters
  CTEST_Exporters.cpp
)

target_link_libraries(
  CTEST_Exporters
  GTest::gtest_main GTest::gmock
  ${PROJECT_LIBRARY_NAME}
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_Exporters
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
