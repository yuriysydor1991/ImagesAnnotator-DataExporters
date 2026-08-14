cmake_minimum_required(VERSION 3.13)

# The factory is compiled into every build of the library, with OpenCV and
# without: it is what answers "did this build get a cropper of its own?" at the
# run time, so that no caller has to repeat the build time condition.
target_sources(
  ${PROJECT_LIBRARY_NAME}
  PRIVATE
    ImageCropperFactory.cpp
)

# The implementation behind it is added only when the OpenCV enabler found the
# library. Without it the factory compiles its nullptr branch and the PyTorch
# Vision export keeps asking its consumer for a cropper.
if (IADE_WITH_OPENCV)
  target_sources(
    ${PROJECT_LIBRARY_NAME}
    PRIVATE
      OpenCVImageCropper.cpp
  )

  target_include_directories(
    ${PROJECT_LIBRARY_NAME}
    PRIVATE ${OpenCV_INCLUDE_DIRS}
  )
endif()

add_subdirectory(tests)
