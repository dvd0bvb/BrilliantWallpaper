#include <format>
#include <type_traits>
#include "ImageProcessing.hpp"
#include "Log.hpp"

namespace brilliant {
  namespace wp {

    ImageInfo::ImageInfo(const ImageInfoType& info) :
      info(info)
    {
    
    }

    ImageInfo::ImageInfo(const std::filesystem::path& path, const ImageTags& tags) :
      tags(tags),
      info(getImageInfo(path, tags))
    {

    }

    std::uint32_t ImageInfo::height() const
    {
      return std::visit([](const auto& i) {
        return static_cast<std::uint32_t>(i._info._height);
      }, info);
    }

    std::uint32_t ImageInfo::width() const
    {
      return std::visit([](const auto& i) {
        return static_cast<std::uint32_t>(i._info._width);
      }, info);
    }

    ImageTags ImageInfo::getType() const
    {
      return tags;
    }

    ImageInfoType ImageInfo::getImageInfo(const std::filesystem::path& path, const ImageTags& tags)
    {
      return std::visit([path](const auto& type) -> ImageInfoType {
        return boost::gil::read_image_info(path, type);
      }, tags);
    }

    std::optional<ImageTags> getImageTypeFromExt(const std::filesystem::path& path)
    {
      const auto ext = path.extension();
      if (ext == ".jpg" || ext == ".jpeg")
      {
        return boost::gil::jpeg_tag{};
      }
      else if (ext == ".bmp")
      {
        return boost::gil::bmp_tag{};
      }
      else if (ext == ".png")
      {
        return boost::gil::png_tag{};
      }
      else if (ext == ".pbm" || ext == ".pgm" || ext == ".ppm")
      {
        return boost::gil::pnm_tag{};
      }

      return std::nullopt;
    }

    std::pair<std::uint32_t, std::uint32_t> getScaledDimsFromHeight(std::uint32_t srcHeight, const ImageInfo& info)
    {
      if (info.height() > srcHeight)
      {
        const double h = info.height();
        const double w = info.width();
        const double srcH = static_cast<double>(srcHeight);

        const std::uint32_t resizedWidth = static_cast<std::uint32_t>(w * (srcH / h));

        log(severity_level::trace, "original dims: {} x {}; resized dims: {} x {}", info.height(), info.width(), srcHeight, resizedWidth);
        return { resizedWidth, srcHeight };
      }
      log(severity_level::debug, "original dims: {} x {}; not resized", info.height(), info.width());
      return { info.width(), info.height() };
    }

    
  }
}