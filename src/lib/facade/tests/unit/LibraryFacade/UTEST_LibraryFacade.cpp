#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "LibraryFacade.h"
#include "src/lib/libmain/LibFactory.h"
#include "src/lib/libmain/LibMain.h"

using namespace ImagesAnnotatorDataExporters011;
using namespace iade0impl;
using namespace testing;

namespace
{

/**
 * @brief The concrete context the facade is asked to forward. Building a real
 * one would drag its exporter into this unit test, which only checks the
 * forwarding itself.
 */
class ContextStub : public LibraryContext
{
 public:
  IExporterPtr create_exporter() const override { return {}; }
};

}  // namespace

class UTEST_LibraryFacade : public Test
{
 public:
  using LibFactory = iade0impl::LibFactory;

  UTEST_LibraryFacade() = default;
  ~UTEST_LibraryFacade() = default;
};

TEST_F(UTEST_LibraryFacade, create_default_lib_success)
{
  MockFunction<void(LibFactory&)> mockEnsurer;

  EXPECT_CALL(mockEnsurer, Call)
      .Times(1)
      .WillOnce(Invoke([](LibFactory& instance) {
        EXPECT_CALL(instance, create_appropriate_lib(_)).Times(1);
      }));

  LibFactory::onMockCreate = mockEnsurer.AsStdFunction();

  auto res = LibraryFacade::create_library({});
}

TEST_F(UTEST_LibraryFacade, create_default_lib_direct_success)
{
  MockFunction<void(LibFactory&)> mockEnsurer;

  EXPECT_CALL(mockEnsurer, Call)
      .Times(1)
      .WillOnce(Invoke([](LibFactory& instance) {
        EXPECT_CALL(instance, create_default_lib()).Times(1);
      }));

  LibFactory::onMockCreate = mockEnsurer.AsStdFunction();

  auto res = LibraryFacade::create_default_lib();
}

TEST_F(UTEST_LibraryFacade, create_library_context_success)
{
  MockFunction<void(LibFactory&)> mockEnsurer;

  EXPECT_CALL(mockEnsurer, Call)
      .Times(1)
      .WillOnce(Invoke([](LibFactory& instance) {
        EXPECT_CALL(instance, create_default_context()).Times(1);
      }));

  LibFactory::onMockCreate = mockEnsurer.AsStdFunction();

  auto res = LibraryFacade::create_library_context();
}

TEST_F(UTEST_LibraryFacade, create_export_context_success)
{
  MockFunction<void(LibFactory&)> mockEnsurer;

  EXPECT_CALL(mockEnsurer, Call)
      .Times(1)
      .WillOnce(Invoke([](LibFactory& instance) {
        EXPECT_CALL(instance, create_export_context()).Times(1);
      }));

  LibFactory::onMockCreate = mockEnsurer.AsStdFunction();

  auto res = LibraryFacade::create_export_context();
}

TEST_F(UTEST_LibraryFacade, create_exporter_forwards_the_given_context)
{
  MockFunction<void(LibFactory&)> mockEnsurer;
  const LibraryContextPtr ctx = std::make_shared<ContextStub>();

  EXPECT_CALL(mockEnsurer, Call)
      .Times(1)
      .WillOnce(Invoke([ctx](LibFactory& instance) {
        EXPECT_CALL(instance, create_exporter(ctx)).Times(1);
      }));

  LibFactory::onMockCreate = mockEnsurer.AsStdFunction();

  auto res = LibraryFacade::create_exporter(ctx);
}

TEST_F(UTEST_LibraryFacade, library_version_is_not_empty)
{
  LibFactory::onMockCreate = {};

  EXPECT_FALSE(LibraryFacade::library_version().empty());
}
