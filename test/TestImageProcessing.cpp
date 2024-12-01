/**
 *
 *  @file      TestImageProcessing.cpp
 *  @author    David Brill
 *  @copyright © David Brill, 2024. All right reserved.
 *
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ImageProcessing.hpp"

template <class VariantType, class T, std::size_t index = 0>
constexpr std::size_t indexOf() {
  if constexpr (index == std::variant_size_v<VariantType>) {
    return index;
  } else if constexpr (std::is_same_v<
                           std::variant_alternative_t<index, VariantType>, T>) {
    return index;
  } else {
    return indexOf<VariantType, T, index + 1>();
  }
}

TEST(TestImageProcessing, testGetImageTypeJpg) {
  auto jpg = brilliant::wp::getImageType("files/test.jpg");
  EXPECT_TRUE(jpg.has_value());
  EXPECT_EQ(jpg.value().index(),
            (indexOf<brilliant::wp::ImageTags, boost::gil::jpeg_tag>()));
}

TEST(TestImageProcessing, testGetImageTypePng) {
  auto png = brilliant::wp::getImageType("files/test.png");
  EXPECT_TRUE(png.has_value());
  EXPECT_EQ(png.value().index(),
            (indexOf<brilliant::wp::ImageTags, boost::gil::png_tag>()));
}

TEST(TestImageProcessing, testGetImageTypeBmp) {
  auto bmp = brilliant::wp::getImageType("files/test.bmp");
  EXPECT_TRUE(bmp.has_value());
  EXPECT_EQ(bmp.value().index(),
            (indexOf<brilliant::wp::ImageTags, boost::gil::bmp_tag>()));
}

TEST(TestImageProcessing, testImageInfo) {
  brilliant::wp::ImageInfo info("files/test.bmp", boost::gil::bmp_tag{});
  EXPECT_EQ(
      info.getType().index(),
      (indexOf<brilliant::wp::ImageInfoType,
               boost::gil::get_reader_backend<const std::filesystem::path,
                                              boost::gil::bmp_tag>::type>()));
  EXPECT_EQ(info.height(), 361);
  EXPECT_EQ(info.width(), 410);
}