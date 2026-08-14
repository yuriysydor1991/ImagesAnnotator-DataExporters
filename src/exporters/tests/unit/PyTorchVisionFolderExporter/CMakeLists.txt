cmake_minimum_required(VERSION 3.13)

add_executable(
  UTEST_PyTorchVisionFolderExporter
  UTEST_PyTorchVisionFolderExporter.cpp
  ${CMAKE_SOURCE_DIR}/src/exporters/PyTorchVisionFolderExporter.cpp
  ${IADE_HELPERS_SRC}
  ${IADE_CURL_SRC}
  ${IADE_CROPPERS_SRC}
)

target_include_directories(
  UTEST_PyTorchVisionFolderExporter
  PRIVATE ${GENERAL_MOCKS_ROOT}/log
  PRIVATE ${GENERAL_MOCKS_ROOT}/project-global-decls
  PRIVATE ${CMAKE_SOURCE_DIR}
  PRIVATE ${IADE_PUBLIC_INCLUDE_DIR}
  PRIVATE ${IADE_OPENCV_TEST_INCLUDES}
)

target_link_libraries(
  UTEST_PyTorchVisionFolderExporter
  GTest::gtest_main GTest::gmock
  ${IMAGES_ANNOTATOR_DATA_DRIVERS_TARGET} CURL::libcurl ${IADE_OPENCV_TEST_LIBS}
)

target_compile_definitions(
  UTEST_PyTorchVisionFolderExporter
  PRIVATE ${IADE_OPENCV_TEST_DEFS}
)

include(GoogleTest)

gtest_add_tests(
  TARGET UTEST_PyTorchVisionFolderExporter
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
