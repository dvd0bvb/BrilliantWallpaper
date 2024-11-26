#pragma once

#include <vector>
#include <variant>
#include <filesystem>
#include <optional>

#include <boost/gil.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/extension/io/bmp.hpp>
#include <boost/gil/extension/io/pnm.hpp>
//TODO: webp?

namespace brilliant {
  namespace wp {

    using ImageType = boost::gil::any_image<
      boost::gil::rgb8_image_t,
      boost::gil::rgba8_image_t,
      boost::gil::rgba16_image_t,
      boost::gil::gray8_image_t,
      boost::gil::gray16_image_t,
      boost::gil::gray_alpha8_image_t, 
      boost::gil::gray_alpha16_image_t>;

    using ImageTags = std::variant<
      boost::gil::jpeg_tag,
      boost::gil::bmp_tag,
      boost::gil::png_tag,
      boost::gil::pnm_tag>;

    namespace {
      template<class T>
      using BackendType = boost::gil::get_reader_backend<const std::filesystem::path, T>::type;

      template<class T>
      struct ImageInfoTypeGenerator;

      //specialize on some template type
      //could have specialized on std::variant<Ts...> but this is more generic
      template<template <class...> class T, class... Ts>
      struct ImageInfoTypeGenerator<T<Ts...>>
      {
        using type = std::variant<BackendType<Ts>...>;
      };

      template<class T>
      using ImageInfoTypeGenerator_t = ImageInfoTypeGenerator<T>::type;
    }

    using ImageInfoType = ImageInfoTypeGenerator_t<ImageTags>;

    class ImageInfo {
    public:
      ImageInfo(const ImageInfoType& info);

      ImageInfo(const std::filesystem::path& path, const ImageTags& tags);

      std::uint32_t height() const;

      std::uint32_t width() const;

      ImageTags getType() const;

    private:
      static ImageInfoType getImageInfo(const std::filesystem::path& path, const ImageTags& tags);

      ImageTags tags;

      ImageInfoType info;
    };

    //TODO: get a better way to id image type
    std::optional<ImageTags> getImageTypeFromExt(const std::filesystem::path& path);

    std::pair<std::uint32_t, std::uint32_t> getScaledDimsFromHeight(std::uint32_t srcHeight, const ImageInfo& info);

    template<class View, class RangeOfImageInfo>
    auto determineRoisFromImageInfo(const View& src, const RangeOfImageInfo& info)
    {      
      std::uint32_t widthOfAllPx = 0;

      std::vector<std::pair<std::uint32_t, std::uint32_t>> scaledView;
      for (const auto& i : info)
      {
        auto scaled = getScaledDimsFromHeight(static_cast<std::uint32_t>(src.height()), i);
        auto checkedWidth = widthOfAllPx + scaled.first;
        if (checkedWidth > src.width())
        {
          break;
        }
        widthOfAllPx = checkedWidth;
        scaledView.emplace_back(std::move(scaled));
      }

      const std::uint32_t widthSpacingPx = (static_cast<std::uint32_t>(src.width()) - widthOfAllPx) / static_cast<std::uint32_t>(scaledView.size() + 1); //+1 for outside edge
      
      using point_t = typename View::point_t;
      using RoiType = std::pair<point_t, point_t>;
      std::vector<RoiType> rois;
      std::uint32_t widthSoFar = 0;

      for (const auto& dims : scaledView) 
      {
        widthSoFar += widthSpacingPx;
        std::uint32_t heightDiffPx = 0;
        if (dims.second < src.height())
        {
          //center image in y dimension
          heightDiffPx = (static_cast<std::uint32_t>(src.height()) - dims.second) / 2;
        }
        rois.emplace_back(point_t{ widthSoFar, heightDiffPx }, point_t{ dims.first + widthSoFar, dims.second + heightDiffPx});
        widthSoFar += dims.first;
      }
      return rois;
    }
  }
}