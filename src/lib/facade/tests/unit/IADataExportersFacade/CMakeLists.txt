cmake_minimum_required(VERSION 3.13)

add_executable(
  UTEST_IADataExportersFacade
  UTEST_IADataExportersFacade.cpp
  ${CMAKE_SOURCE_DIR}/src/lib/facade/IADataExportersFacade.cpp
  ${CMAKE_SOURCE_DIR}/src/log/default-logger/DefaultLogger.cpp
  ${CMAKE_SOURCE_DIR}/src/log/default-logger/real-default-logger/RealDefaultLogger.cpp
)

target_include_directories(
  UTEST_IADataExportersFacade
  PRIVATE ${GENERAL_MOCKS_ROOT}/LibMain
  PRIVATE ${GENERAL_MOCKS_ROOT}/project-global-decls
  PRIVATE ${FACADE_MOCK_DIR}/LibFactory
  PRIVATE ${CMAKE_SOURCE_DIR}
  PRIVATE ${IADE_PUBLIC_INCLUDE_DIR}
)

target_link_libraries(
  UTEST_IADataExportersFacade
  GTest::gtest_main GTest::gmock ${IMAGES_ANNOTATOR_DATA_DRIVERS_TARGET}
)

include(GoogleTest)

gtest_add_tests(
  TARGET UTEST_IADataExportersFacade
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
