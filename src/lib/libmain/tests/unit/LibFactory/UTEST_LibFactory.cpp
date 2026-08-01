#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "ExportFormat.h"
#include "src/lib/libmain/LibFactory.h"

using namespace ImagesAnnotatorDataExporters011;
using namespace lib0impl;
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

TEST_F(UTEST_LibFactory, create_exporter_gives_an_instance_for_every_format)
{
  EXPECT_NE(factory->create_exporter(ExportFormat::PlainTxt2Folder), nullptr);
  EXPECT_NE(factory->create_exporter(ExportFormat::Yolo42Folder), nullptr);
  EXPECT_NE(factory->create_exporter(ExportFormat::PyTorchVisionFolder),
            nullptr);
}
