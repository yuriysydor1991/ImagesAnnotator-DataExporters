#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

#include "src/lib/libmain/croppers/ImageCropperFactory.h"
#include "src/lib/libmain/croppers/OpenCVImageCropper.h"

using namespace testing;
using iannotator::exporters::ImageRecord;
using iannotator::exporters::ImageRecordPtr;
using iannotator::exporters::ImageRecordRect;
using iannotator::exporters::ImageRecordRectPtr;
using iannotator::exporters::croppers::create_builtin_cropper;
using iannotator::exporters::croppers::OpenCVImageCropper;

// This whole suite is configured only in a build that found OpenCV - see the
// tests/unit/CMakeLists.txt beside it. The build without one is covered by
// UTEST_LibFactory, which asserts the factory hands out no cropper there.
TEST(UTEST_OpenCVImageCropper, builtin_cropper_is_there_in_an_opencv_build)
{
  EXPECT_NE(create_builtin_cropper(), nullptr);
}

namespace
{

namespace fs = std::filesystem;

class UTEST_OpenCVImageCropperF : public Test
{
 public:
  fs::path dir;

  void SetUp() override
  {
    dir = fs::path{testing::TempDir()} / "utest_opencv_cropper";
    fs::remove_all(dir);
    fs::create_directories(dir);
  }

  void TearDown() override { fs::remove_all(dir); }

  /// @brief Writes a solid image of the given size and returns its record.
  ImageRecordPtr given_image(const std::string& name, int width, int height)
  {
    const cv::Mat image{height, width, CV_8UC3, cv::Scalar{10, 20, 30}};

    EXPECT_TRUE(cv::imwrite((dir / name).string(), image));

    return ImageRecord::create(name, dir.string());
  }

  static ImageRecordRectPtr given_rect(const std::string& name, int x, int y,
                                       int width, int height)
  {
    return std::make_shared<ImageRecordRect>(name, x, y, width, height);
  }

  /// @brief The size of the image written at the given path.
  static cv::Size size_of(const std::string& path)
  {
    const cv::Mat written = cv::imread(path, cv::IMREAD_UNCHANGED);

    EXPECT_FALSE(written.empty()) << "not decodable: " << path;

    return written.size();
  }
};

}  // namespace

TEST_F(UTEST_OpenCVImageCropperF, crops_the_rectangle_out_at_its_own_size)
{
  auto ir = given_image("src.png", 100, 50);
  auto irr = given_rect("cat", 10, 5, 20, 30);

  std::string tofpath = (dir / "out.png").string();

  OpenCVImageCropper cropper;

  ASSERT_TRUE(cropper.crop_out_2_fs(ir, irr, tofpath));
  ASSERT_TRUE(fs::is_regular_file(tofpath));

  EXPECT_EQ(size_of(tofpath), cv::Size(20, 30));
}

TEST_F(UTEST_OpenCVImageCropperF, clamps_a_rectangle_reaching_over_the_edge)
{
  auto ir = given_image("src.png", 100, 50);
  // 40 wide from x=80 would reach 20 pixels past the right edge, and 30 high
  // from y=40 ten past the bottom one.
  auto irr = given_rect("dog", 80, 40, 40, 30);

  std::string tofpath = (dir / "out.png").string();

  OpenCVImageCropper cropper;

  ASSERT_TRUE(cropper.crop_out_2_fs(ir, irr, tofpath));

  EXPECT_EQ(size_of(tofpath), cv::Size(20, 10));
}

TEST_F(UTEST_OpenCVImageCropperF, rejects_a_rectangle_outside_of_the_image)
{
  auto ir = given_image("src.png", 100, 50);
  auto irr = given_rect("cat", 200, 200, 10, 10);

  std::string tofpath = (dir / "out.png").string();

  OpenCVImageCropper cropper;

  EXPECT_FALSE(cropper.crop_out_2_fs(ir, irr, tofpath));
}

TEST_F(UTEST_OpenCVImageCropperF, rewrites_the_destination_to_a_png_name)
{
  auto ir = given_image("src.png", 100, 50);
  auto irr = given_rect("cat", 0, 0, 10, 10);

  // The PyTorch Vision export hands over the original file name, whatever
  // extension it carries; the crop is written as a PNG regardless.
  std::string tofpath = (dir / "out.jpeg").string();

  OpenCVImageCropper cropper;

  ASSERT_TRUE(cropper.crop_out_2_fs(ir, irr, tofpath));

  EXPECT_EQ(fs::path{tofpath}.extension().string(), ".png");
  EXPECT_TRUE(fs::is_regular_file(tofpath));
}

TEST_F(UTEST_OpenCVImageCropperF, never_overwrites_an_already_written_crop)
{
  auto ir = given_image("src.png", 100, 50);
  auto irr = given_rect("cat", 0, 0, 10, 10);

  const std::string wanted = (dir / "out.png").string();

  OpenCVImageCropper cropper;

  std::string first = wanted;
  ASSERT_TRUE(cropper.crop_out_2_fs(ir, irr, first));

  std::string second = wanted;
  ASSERT_TRUE(cropper.crop_out_2_fs(ir, irr, second));

  EXPECT_EQ(first, wanted);
  EXPECT_NE(second, first);
  EXPECT_TRUE(fs::is_regular_file(first));
  EXPECT_TRUE(fs::is_regular_file(second));
}

TEST_F(UTEST_OpenCVImageCropperF, fails_on_an_image_it_cannot_decode)
{
  std::ofstream{dir / "junk.png"} << "not an image at all";

  auto ir = ImageRecord::create("junk.png", dir.string());
  auto irr = given_rect("cat", 0, 0, 10, 10);

  std::string tofpath = (dir / "out.png").string();

  OpenCVImageCropper cropper;

  EXPECT_FALSE(cropper.crop_out_2_fs(ir, irr, tofpath));
}

TEST_F(UTEST_OpenCVImageCropperF, fails_on_a_missing_image_file)
{
  auto ir = ImageRecord::create("absent.png", dir.string());
  auto irr = given_rect("cat", 0, 0, 10, 10);

  std::string tofpath = (dir / "out.png").string();

  OpenCVImageCropper cropper;

  EXPECT_FALSE(cropper.crop_out_2_fs(ir, irr, tofpath));
}

TEST_F(UTEST_OpenCVImageCropperF, rejects_an_empty_destination_path)
{
  auto ir = given_image("src.png", 100, 50);
  auto irr = given_rect("cat", 0, 0, 10, 10);

  std::string tofpath;

  OpenCVImageCropper cropper;

  EXPECT_FALSE(cropper.crop_out_2_fs(ir, irr, tofpath));
}

TEST_F(UTEST_OpenCVImageCropperF, clone_produces_a_usable_cropper)
{
  auto ir = given_image("src.png", 100, 50);
  auto irr = given_rect("cat", 0, 0, 10, 10);

  OpenCVImageCropper cropper;

  auto copy = cropper.clone();

  ASSERT_NE(copy, nullptr);

  std::string tofpath = (dir / "out.png").string();

  ASSERT_TRUE(copy->crop_out_2_fs(ir, irr, tofpath));

  EXPECT_EQ(size_of(tofpath), cv::Size(10, 10));
}

