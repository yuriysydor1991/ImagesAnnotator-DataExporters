#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "PlainTxtExportLibraryContext.h"
#include "PyTorchExportLibraryContext.h"
#include "Yolo4ExportLibraryContext.h"
#include "src/lib/libmain/LibFactory.h"

using namespace ImagesAnnotatorDataExporters011;
using namespace iade0impl;
using namespace testing;

class UTEST_LibFactory : public Test
{
 public:
  UTEST_LibFactory() : factory{std::make_shared<LibFactory>()} {}

  std::shared_ptr<LibFactory> factory;
};

TEST_F(UTEST_LibFactory, create_default_lib_success)
{
  EXPECT_NE(factory->create_default_lib(), nullptr);
}

TEST_F(UTEST_LibFactory, create_default_context_success)
{
  EXPECT_NE(factory->create_default_context(), nullptr);
}

TEST_F(UTEST_LibFactory, create_appropriate_lib_success)
{
  EXPECT_NE(factory->create_appropriate_lib({}), nullptr);
}

TEST_F(UTEST_LibFactory, create_export_context_success)
{
  EXPECT_NE(factory->create_export_context(), nullptr);
}

TEST_F(UTEST_LibFactory, create_exporter_gives_an_instance_for_every_context)
{
  EXPECT_NE(
      factory->create_exporter(std::make_shared<PlainTxtExportLibraryContext>()),
      nullptr);
  EXPECT_NE(
      factory->create_exporter(std::make_shared<Yolo4ExportLibraryContext>()),
      nullptr);
  EXPECT_NE(
      factory->create_exporter(std::make_shared<PyTorchExportLibraryContext>()),
      nullptr);
}

TEST_F(UTEST_LibFactory, create_exporter_without_a_context_failure)
{
  EXPECT_EQ(factory->create_exporter({}), nullptr);
}

// The one cropper case that runs in every configuration: with OpenCV the
// factory hands out the library's own cropper, without it a nullptr, which is
// what makes ExportContext::cropper mandatory again. The cropper itself is
// covered by UTEST_OpenCVImageCropper, configured only in an OpenCV build.
TEST_F(UTEST_LibFactory, create_image_cropper_matches_what_the_build_found)
{
  const auto cropper = factory->create_image_cropper();

#ifdef IADE_WITH_OPENCV
  EXPECT_NE(cropper, nullptr);
#else
  EXPECT_EQ(cropper, nullptr);
#endif  // IADE_WITH_OPENCV
}
