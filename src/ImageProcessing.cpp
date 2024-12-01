/**
 *
 *  @file      ImageProcessing.cpp
 *  @author    David Brill
 *  @copyright © David Brill, 2024. All right reserved.
 *
 *  Implements the ImageInfo class
 */
#include "ImageProcessing.hpp"

#include <format>
#include <fstream>
#include <ranges>
#include <type_traits>

#include "Log.hpp"

namespace brilliant {
  namespace wp {

    /**
     * @brief Quick and dirty byte literal operator
     * @param i The number to convert to a byte
     * @return The number converted to a byte
     */
    constexpr std::byte operator"" _bt(unsigned long long int i) {
      return static_cast<std::byte>(i);
    }

    ImageInfo::ImageInfo(const ImageInfoType& info) : info(info) {}

    ImageInfo::ImageInfo(const std::filesystem::path& path,
                         const ImageTags& tags)
        : ImageInfo(getImageInfo(path, tags)) {}

    std::uint32_t ImageInfo::height() const {
      return std::visit(
          [](const auto& i) {
            return static_cast<std::uint32_t>(i._info._height);
          },
          info);
    }

    std::uint32_t ImageInfo::width() const {
      return std::visit(
          [](const auto& i) {
            return static_cast<std::uint32_t>(i._info._width);
          },
          info);
    }

    ImageTags ImageInfo::getType() const {
      return std::visit(
          [](const auto& i) -> ImageTags {
            return std::decay_t<decltype(i)>::format_tag_t();
          },
          info);
    }

    ImageInfoType ImageInfo::getImageInfo(const std::filesystem::path& path,
                                          const ImageTags& tags) {
      return std::visit(
          [path](const auto& type) -> ImageInfoType {
            return boost::gil::read_image_info(path, type);
          },
          tags);
    }

    std::optional<ImageTags> getImageType(const std::filesystem::path& path) {
      constexpr std::array<std::byte, 2> bmpHeader{0x42_bt, 0x4D_bt};
      constexpr std::array<std::byte, 3> jpgHeader{0xff_bt, 0xd8_bt, 0xff_bt};
      constexpr std::array<std::byte, 8> pngHeader = {0x89_bt, 0x50_bt, 0x4e_bt,
                                                      0x47_bt, 0x0d_bt, 0x0a_bt,
                                                      0x1a_bt, 0x0a_bt};
      constexpr std::array<std::byte, 3> pbmHeader1 = {0x50_bt, 0x31_bt,
                                                       0x0a_bt};
      constexpr std::array<std::byte, 3> pbmHeader2 = {0x50_bt, 0x34_bt,
                                                       0x0a_bt};
      constexpr std::array<std::byte, 3> pgmHeader1 = {0x50_bt, 0x32_bt,
                                                       0x0a_bt};
      constexpr std::array<std::byte, 3> pgmHeader2 = {0x50_bt, 0x35_bt,
                                                       0x0a_bt};
      constexpr std::array<std::byte, 3> ppmHeader1 = {0x50_bt, 0x33_bt,
                                                       0x0a_bt};
      constexpr std::array<std::byte, 3> ppmHeader2 = {0x50_bt, 0x36_bt,
                                                       0x0a_bt};

      if (std::basic_ifstream<std::byte> file(path, std::ios::binary); file) {
        std::array<std::byte, 8> bytes{};
        file.read(bytes.data(), bytes.size());

        if (std::ranges::equal(bytes | std::views::take(bmpHeader.size()),
                               bmpHeader)) {
          return boost::gil::bmp_tag{};
        } else if (std::ranges::equal(
                       bytes | std::views::take(jpgHeader.size()), jpgHeader)) {
          return boost::gil::jpeg_tag{};
        } else if (std::ranges::equal(bytes, pngHeader)) {
          return boost::gil::png_tag{};
        } else if (auto firstThreeBytes = bytes | std::views::take(3);
                   std::ranges::equal(firstThreeBytes, pbmHeader1) ||
                   std::ranges::equal(firstThreeBytes, pbmHeader2) ||
                   std::ranges::equal(firstThreeBytes, pgmHeader1) ||
                   std::ranges::equal(firstThreeBytes, pgmHeader2) ||
                   std::ranges::equal(firstThreeBytes, ppmHeader1) ||
                   std::ranges::equal(firstThreeBytes, ppmHeader2)) {
          return boost::gil::pnm_tag{};
        }
      }
      return std::nullopt;
    }

    std::pair<std::uint32_t, std::uint32_t> getScaledDimsFromHeight(
        std::uint32_t srcHeight, const ImageInfo& info) {
      if (info.height() > srcHeight) {
        const double h = info.height();
        const double w = info.width();
        const double srcH = static_cast<double>(srcHeight);

        const std::uint32_t resizedWidth =
            static_cast<std::uint32_t>(w * (srcH / h));

        log(severity_level::trace,
            "original dims: {} x {}; resized dims: {} x {}", info.height(),
            info.width(), srcHeight, resizedWidth);
        return {resizedWidth, srcHeight};
      }
      log(severity_level::trace, "original dims: {} x {}; not resized",
          info.height(), info.width());
      return {info.width(), info.height()};
    }

  }  // namespace wp
}  // namespace brilliant