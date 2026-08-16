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
  MOCK_METHOD(bool, export_db, (LibraryContextPtr ctx), (override));
};

}  // namespace

class UTEST_LibMain : public Test
{
 public:
  UTEST_LibMain() : libmain{std::make_shared<LibMain>()} {}

  ~UTEST_LibMain() { LibFactory::onMockCreate = {}; }

  LibraryContextPtr filled_context()
  {
    auto ctx = std::make_shared<LibraryContext>();
    ctx->set_export_path("/tmp/some-export-dir");
    return ctx;
  }

  std::shared_ptr<LibMain> libmain;
};

TEST_F(UTEST_LibMain, perform_export_no_context_failure)
{
  LibFactory::onMockCreate = {};

  EXPECT_FALSE(libmain->perform_export({}));
}

TEST_F(UTEST_LibMain, perform_export_no_exporter_for_the_context_failure)
{
  LibFactory::onMockCreate = [](LibFactory& instance) {
    EXPECT_CALL(instance, create_exporter(_))
        .Times(1)
        .WillOnce(Return(IExporterPtr{}));
  };

  EXPECT_FALSE(libmain->perform_export(filled_context()));
}

TEST_F(UTEST_LibMain, perform_export_reports_the_exporter_failure)
{
  auto exporter = std::make_shared<ExporterMock>();
  auto ctx = filled_context();

  EXPECT_CALL(*exporter, export_db(_)).Times(1).WillOnce(Return(false));

  LibFactory::onMockCreate = [exporter, ctx](LibFactory& instance) {
    EXPECT_CALL(instance, create_exporter(ctx))
        .Times(1)
        .WillOnce(Return(exporter));
  };

  EXPECT_FALSE(libmain->perform_export(ctx));
}

TEST_F(UTEST_LibMain,
       perform_export_success_publishes_the_exporter_and_passes_the_context)
{
  auto exporter = std::make_shared<ExporterMock>();
  auto ctx = filled_context();

  EXPECT_CALL(*exporter, export_db(ctx)).Times(1).WillOnce(Return(true));

  LibFactory::onMockCreate = [exporter, ctx](LibFactory& instance) {
    EXPECT_CALL(instance, create_exporter(ctx))
        .Times(1)
        .WillOnce(Return(exporter));
  };

  EXPECT_TRUE(libmain->perform_export(ctx));

  EXPECT_EQ(ctx->get_exporter(), exporter);

  // The published field holds the exporter and the expectation above holds
  // the context, so that pointer cycle has to be broken for the mock to be
  // freed and verified at all.
  ctx->set_exporter({});
}
