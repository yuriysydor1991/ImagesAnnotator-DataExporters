cmake_minimum_required(VERSION 3.13)

# Declared in every configuration. The suite tests the cropper itself only when
# the build found OpenCV; without it there is still the one case asserting that
# the factory hands out no cropper - see UTEST_OpenCVImageCropper.cpp.
add_executable(
  UTEST_OpenCVImageCropper
  UTEST_OpenCVImageCropper.cpp
  ${IADE_CROPPERS_SRC}
)

target_include_directories(
  UTEST_OpenCVImageCropper
  PRIVATE ${GENERAL_MOCKS_ROOT}/log
  PRIVATE ${GENERAL_MOCKS_ROOT}/project-global-decls
  PRIVATE ${CMAKE_SOURCE_DIR}
  PRIVATE ${IADE_PUBLIC_INCLUDE_DIR}
  PRIVATE ${IADE_OPENCV_TEST_INCLUDES}
)

target_link_libraries(
  UTEST_OpenCVImageCropper
  GTest::gtest_main GTest::gmock
  ${IMAGES_ANNOTATOR_DATA_DRIVERS_TARGET} ${IADE_OPENCV_TEST_LIBS}
)

target_compile_definitions(
  UTEST_OpenCVImageCropper
  PRIVATE ${IADE_OPENCV_TEST_DEFS}
)

include(GoogleTest)

gtest_add_tests(
  TARGET UTEST_OpenCVImageCropper
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
