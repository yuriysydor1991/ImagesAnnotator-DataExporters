#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "IExporter.h"
#include "LibraryContext.h"
#include "src/lib/libmain/LibFactory.h"
#include "src/lib/libmain/LibMain.h"

using namespace ImagesAnnotatorDataExporters011;
using namespace iade0impl;
using namespace testing;

namespace
{

class ExporterMock : public IExporter
{
 public:
  MOCK_METHOD(bool, export_db, (ExportContextPtr ectx), (override));
};

/**
 * @brief The concrete context every libcall needs. LibMain asks the factory
 * for the exporter, so which layout the context stands for never reaches it.
 */
class ContextStub : public LibraryContext
{
 public:
  IExporterPtr create_exporter() const override { return {}; }
};

}  // namespace

class UTEST_LibMain : public Test
{
 public:
  UTEST_LibMain() : libmain{std::make_shared<LibMain>()} {}

  ~UTEST_LibMain() { LibFactory::onMockCreate = {}; }

  LibraryContextPtr filled_context()
  {
    auto ctx = std::make_shared<ContextStub>();
    ctx->export_path = "/tmp/some-export-dir";
    return ctx;
  }

  std::shared_ptr<LibMain> libmain;
};

TEST_F(UTEST_LibMain, libcall_no_context_failure)
{
  LibFactory::onMockCreate = {};

  EXPECT_FALSE(libmain->libcall({}));
}

TEST_F(UTEST_LibMain, libcall_no_exporter_for_the_format_failure)
{
  LibFactory::onMockCreate = [](LibFactory& instance) {
    EXPECT_CALL(instance, create_exporter(_))
        .Times(1)
        .WillOnce(Return(IExporterPtr{}));
  };

  EXPECT_FALSE(libmain->libcall(filled_context()));
}

TEST_F(UTEST_LibMain, libcall_reports_the_exporter_failure)
{
  auto exporter = std::make_shared<ExporterMock>();
  auto ctx = filled_context();

  EXPECT_CALL(*exporter, export_db(_)).Times(1).WillOnce(Return(false));

  LibFactory::onMockCreate = [exporter, ctx](LibFactory& instance) {
    EXPECT_CALL(instance, create_exporter(ctx))
        .Times(1)
        .WillOnce(Return(exporter));
    EXPECT_CALL(instance, create_export_context())
        .Times(1)
        .WillOnce(Return(std::make_shared<ExportContext>()));
  };

  EXPECT_FALSE(libmain->libcall(ctx));
}

TEST_F(UTEST_LibMain, libcall_success_publishes_the_exporter_and_the_context)
{
  auto exporter = std::make_shared<ExporterMock>();
  auto ectx = std::make_shared<ExportContext>();
  auto ctx = filled_context();

  EXPECT_CALL(*exporter, export_db(ectx)).Times(1).WillOnce(Return(true));

  LibFactory::onMockCreate = [exporter, ectx, ctx](LibFactory& instance) {
    EXPECT_CALL(instance, create_exporter(ctx))
        .Times(1)
        .WillOnce(Return(exporter));
    EXPECT_CALL(instance, create_export_context())
        .Times(1)
        .WillOnce(Return(ectx));
  };

  EXPECT_TRUE(libmain->libcall(ctx));

  EXPECT_EQ(ctx->exporter, exporter);
  EXPECT_EQ(ectx->export_path, ctx->export_path);
}
