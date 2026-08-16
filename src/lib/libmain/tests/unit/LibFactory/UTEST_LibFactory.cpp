#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "LibraryContext.h"
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

TEST_F(UTEST_LibFactory, create_library_context_of_every_layout_success)
{
  EXPECT_NE(factory->create_plain_txt_library_context(), nullptr);
  EXPECT_NE(factory->create_yolo4_library_context(), nullptr);
  EXPECT_NE(factory->create_pytorch_library_context(), nullptr);
}

// Every layout context has to name its own exporter, which is the whole reason
// the factory hands out one method per layout instead of a default context.
TEST_F(UTEST_LibFactory, every_created_context_names_its_own_exporter)
{
  EXPECT_NE(
      factory->create_exporter(factory->create_plain_txt_library_context()),
      nullptr);
  EXPECT_NE(factory->create_exporter(factory->create_yolo4_library_context()),
            nullptr);
  EXPECT_NE(factory->create_exporter(factory->create_pytorch_library_context()),
            nullptr);
}

TEST_F(UTEST_LibFactory, create_appropriate_lib_success)
{
  EXPECT_NE(factory->create_appropriate_lib({}), nullptr);
}

TEST_F(UTEST_LibFactory, create_exporter_gives_an_instance_for_every_context)
{
  EXPECT_NE(factory->create_exporter(
                std::make_shared<PlainTxtExportLibraryContext>()),
            nullptr);
  EXPECT_NE(
      factory->create_exporter(std::make_shared<Yolo4ExportLibraryContext>()),
      nullptr);
  EXPECT_NE(
      factory->create_exporter(std::make_shared<PyTorchExportLibraryContext>()),
      nullptr);
}

TEST_F(UTEST_LibFactory, create_exporter_without_a_layout_context_failure)
{
  EXPECT_EQ(factory->create_exporter({}), nullptr);
  EXPECT_EQ(factory->create_exporter(std::make_shared<LibraryContext>()),
            nullptr);
}

// The one cropper case that runs in every configuration: with OpenCV the
// factory hands out the library's own cropper, without it a nullptr, which is
// what makes PyTorchExportLibraryContext::set_cropper() mandatory again. The
// cropper itself is covered by UTEST_OpenCVImageCropper, configured only in an
// OpenCV build.
TEST_F(UTEST_LibFactory, create_image_cropper_matches_what_the_build_found)
{
  const auto cropper = factory->create_image_cropper();

#ifdef IADE_WITH_OPENCV
  EXPECT_NE(cropper, nullptr);
#else
  EXPECT_EQ(cropper, nullptr);
#endif  // IADE_WITH_OPENCV
}
