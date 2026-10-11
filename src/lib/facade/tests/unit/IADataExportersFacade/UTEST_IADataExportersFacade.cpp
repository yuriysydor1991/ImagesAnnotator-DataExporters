#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "IADataExportersFacade.h"
#include "src/lib/libmain/LibFactory.h"
#include "src/lib/libmain/LibMain.h"

using namespace ImagesAnnotatorDataExporters012;
using namespace iade0impl;
using namespace testing;

class UTEST_IADataExportersFacade : public Test
{
 public:
  using LibFactory = iade0impl::LibFactory;

  UTEST_IADataExportersFacade() = default;
  ~UTEST_IADataExportersFacade() = default;
};

TEST_F(UTEST_IADataExportersFacade, create_default_lib_success)
{
  MockFunction<void(LibFactory&)> mockEnsurer;

  EXPECT_CALL(mockEnsurer, Call)
      .Times(1)
      .WillOnce(Invoke([](LibFactory& instance) {
        EXPECT_CALL(instance, create_appropriate_lib(_)).Times(1);
      }));

  LibFactory::onMockCreate = mockEnsurer.AsStdFunction();

  auto res = IADataExportersFacade::create_library({});
}

TEST_F(UTEST_IADataExportersFacade, create_default_lib_direct_success)
{
  MockFunction<void(LibFactory&)> mockEnsurer;

  EXPECT_CALL(mockEnsurer, Call)
      .Times(1)
      .WillOnce(Invoke([](LibFactory& instance) {
        EXPECT_CALL(instance, create_default_lib()).Times(1);
      }));

  LibFactory::onMockCreate = mockEnsurer.AsStdFunction();

  auto res = IADataExportersFacade::create_default_lib();
}

TEST_F(UTEST_IADataExportersFacade, create_plain_txt_library_context_success)
{
  MockFunction<void(LibFactory&)> mockEnsurer;

  EXPECT_CALL(mockEnsurer, Call)
      .Times(1)
      .WillOnce(Invoke([](LibFactory& instance) {
        EXPECT_CALL(instance, create_plain_txt_library_context()).Times(1);
      }));

  LibFactory::onMockCreate = mockEnsurer.AsStdFunction();

  auto res = IADataExportersFacade::create_plain_txt_library_context();
}

TEST_F(UTEST_IADataExportersFacade, create_yolo4_library_context_success)
{
  MockFunction<void(LibFactory&)> mockEnsurer;

  EXPECT_CALL(mockEnsurer, Call)
      .Times(1)
      .WillOnce(Invoke([](LibFactory& instance) {
        EXPECT_CALL(instance, create_yolo4_library_context()).Times(1);
      }));

  LibFactory::onMockCreate = mockEnsurer.AsStdFunction();

  auto res = IADataExportersFacade::create_yolo4_library_context();
}

TEST_F(UTEST_IADataExportersFacade,
       create_ultralytics_detect_library_context_success)
{
  MockFunction<void(LibFactory&)> mockEnsurer;

  EXPECT_CALL(mockEnsurer, Call)
      .Times(1)
      .WillOnce(Invoke([](LibFactory& instance) {
        EXPECT_CALL(instance, create_ultralytics_detect_library_context())
            .Times(1);
      }));

  LibFactory::onMockCreate = mockEnsurer.AsStdFunction();

  auto res = IADataExportersFacade::create_ultralytics_detect_library_context();
}

TEST_F(UTEST_IADataExportersFacade,
       create_ultralytics_obb_library_context_success)
{
  MockFunction<void(LibFactory&)> mockEnsurer;

  EXPECT_CALL(mockEnsurer, Call)
      .Times(1)
      .WillOnce(Invoke([](LibFactory& instance) {
        EXPECT_CALL(instance, create_ultralytics_obb_library_context())
            .Times(1);
      }));

  LibFactory::onMockCreate = mockEnsurer.AsStdFunction();

  auto res = IADataExportersFacade::create_ultralytics_obb_library_context();
}

TEST_F(UTEST_IADataExportersFacade,
       create_ultralytics_segment_library_context_success)
{
  MockFunction<void(LibFactory&)> mockEnsurer;

  EXPECT_CALL(mockEnsurer, Call)
      .Times(1)
      .WillOnce(Invoke([](LibFactory& instance) {
        EXPECT_CALL(instance, create_ultralytics_segment_library_context())
            .Times(1);
      }));

  LibFactory::onMockCreate = mockEnsurer.AsStdFunction();

  auto res =
      IADataExportersFacade::create_ultralytics_segment_library_context();
}

TEST_F(UTEST_IADataExportersFacade, create_coco_library_context_success)
{
  MockFunction<void(LibFactory&)> mockEnsurer;

  EXPECT_CALL(mockEnsurer, Call)
      .Times(1)
      .WillOnce(Invoke([](LibFactory& instance) {
        EXPECT_CALL(instance, create_coco_library_context()).Times(1);
      }));

  LibFactory::onMockCreate = mockEnsurer.AsStdFunction();

  auto res = IADataExportersFacade::create_coco_library_context();
}

TEST_F(UTEST_IADataExportersFacade, create_pascal_voc_library_context_success)
{
  MockFunction<void(LibFactory&)> mockEnsurer;

  EXPECT_CALL(mockEnsurer, Call)
      .Times(1)
      .WillOnce(Invoke([](LibFactory& instance) {
        EXPECT_CALL(instance, create_pascal_voc_library_context()).Times(1);
      }));

  LibFactory::onMockCreate = mockEnsurer.AsStdFunction();

  auto res = IADataExportersFacade::create_pascal_voc_library_context();
}

TEST_F(UTEST_IADataExportersFacade, create_createml_library_context_success)
{
  MockFunction<void(LibFactory&)> mockEnsurer;

  EXPECT_CALL(mockEnsurer, Call)
      .Times(1)
      .WillOnce(Invoke([](LibFactory& instance) {
        EXPECT_CALL(instance, create_createml_library_context()).Times(1);
      }));

  LibFactory::onMockCreate = mockEnsurer.AsStdFunction();

  auto res = IADataExportersFacade::create_createml_library_context();
}

TEST_F(UTEST_IADataExportersFacade, create_dxxwjz2_library_context_success)
{
  MockFunction<void(LibFactory&)> mockEnsurer;

  EXPECT_CALL(mockEnsurer, Call)
      .Times(1)
      .WillOnce(Invoke([](LibFactory& instance) {
        EXPECT_CALL(instance, create_dxxwjz2_library_context()).Times(1);
      }));

  LibFactory::onMockCreate = mockEnsurer.AsStdFunction();

  auto res = IADataExportersFacade::create_dxxwjz2_library_context();
}

TEST_F(UTEST_IADataExportersFacade, create_dxxwjz3_library_context_success)
{
  MockFunction<void(LibFactory&)> mockEnsurer;

  EXPECT_CALL(mockEnsurer, Call)
      .Times(1)
      .WillOnce(Invoke([](LibFactory& instance) {
        EXPECT_CALL(instance, create_dxxwjz3_library_context()).Times(1);
      }));

  LibFactory::onMockCreate = mockEnsurer.AsStdFunction();

  auto res = IADataExportersFacade::create_dxxwjz3_library_context();
}

TEST_F(UTEST_IADataExportersFacade, create_dxxwjz2p_library_context_success)
{
  MockFunction<void(LibFactory&)> mockEnsurer;

  EXPECT_CALL(mockEnsurer, Call)
      .Times(1)
      .WillOnce(Invoke([](LibFactory& instance) {
        EXPECT_CALL(instance, create_dxxwjz2p_library_context()).Times(1);
      }));

  LibFactory::onMockCreate = mockEnsurer.AsStdFunction();

  auto res = IADataExportersFacade::create_dxxwjz2p_library_context();
}

TEST_F(UTEST_IADataExportersFacade, create_dxxwjz3p_library_context_success)
{
  MockFunction<void(LibFactory&)> mockEnsurer;

  EXPECT_CALL(mockEnsurer, Call)
      .Times(1)
      .WillOnce(Invoke([](LibFactory& instance) {
        EXPECT_CALL(instance, create_dxxwjz3p_library_context()).Times(1);
      }));

  LibFactory::onMockCreate = mockEnsurer.AsStdFunction();

  auto res = IADataExportersFacade::create_dxxwjz3p_library_context();
}

TEST_F(UTEST_IADataExportersFacade, create_pytorch_library_context_success)
{
  MockFunction<void(LibFactory&)> mockEnsurer;

  EXPECT_CALL(mockEnsurer, Call)
      .Times(1)
      .WillOnce(Invoke([](LibFactory& instance) {
        EXPECT_CALL(instance, create_pytorch_library_context()).Times(1);
      }));

  LibFactory::onMockCreate = mockEnsurer.AsStdFunction();

  auto res = IADataExportersFacade::create_pytorch_library_context();
}

TEST_F(UTEST_IADataExportersFacade, create_exporter_forwards_the_given_context)
{
  MockFunction<void(LibFactory&)> mockEnsurer;
  const IADataExportersContextPtr ctx =
      std::make_shared<IADataExportersContext>();

  EXPECT_CALL(mockEnsurer, Call)
      .Times(1)
      .WillOnce(Invoke([ctx](LibFactory& instance) {
        EXPECT_CALL(instance, create_exporter(ctx)).Times(1);
      }));

  LibFactory::onMockCreate = mockEnsurer.AsStdFunction();

  auto res = IADataExportersFacade::create_exporter(ctx);
}

TEST_F(UTEST_IADataExportersFacade, library_version_is_not_empty)
{
  LibFactory::onMockCreate = {};

  EXPECT_FALSE(IADataExportersFacade::library_version().empty());
}
