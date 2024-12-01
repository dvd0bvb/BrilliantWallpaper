/**
 *
 *  @file      ImageProcessing.hpp
 *  @author    David Brill
 *  @copyright © David Brill, 2024. All right reserved.
 *
 *  Defines types and functions for image processing
 */
#pragma once

#include <boost/gil.hpp>
#include <boost/gil/extension/io/bmp.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/extension/io/pnm.hpp>
#include <filesystem>
#include <optional>
#include <variant>
#include <vector>
// TODO: webp?

namespace brilliant {
  namespace wp {

    //! A union type for various image types used by boost::gil
    using ImageType = boost::gil::any_image<
        boost::gil::rgb8_image_t, boost::gil::rgba8_image_t,
        boost::gil::rgba16_image_t, boost::gil::gray8_image_t,
        boost::gil::gray16_image_t, boost::gil::gray_alpha8_image_t,
        boost::gil::gray_alpha16_image_t>;

    //! A union type for image formats
    using ImageTags = std::variant<boost::gil::jpeg_tag, boost::gil::bmp_tag,
                                   boost::gil::png_tag, boost::gil::pnm_tag>;

    namespace {

      //! Convenience type for boost::gil reader backends
      template <class T>
      using BackendType =
          boost::gil::get_reader_backend<const std::filesystem::path, T>::type;

      /**
       * @brief Base template for the ImageInfoTypeGenerator
       * @tparam T A type from ImageTags
       */
      template <class T>
      struct ImageInfoTypeGenerator;

      /**
       * @brief Generate backend types
       * @tparam T A template class type
       * @tparam ...Ts Image tag types given by template args in T
       *
       * Generates a variant containing boost::gil reader backend types from
       * the types contained in ImageTags. Allows for changing ImageTags without
       * the need to manually update the backend reader variant as well.
       */
      template <template <class...> class T, class... Ts>
      struct ImageInfoTypeGenerator<T<Ts...>> {
        using type = std::variant<BackendType<Ts>...>;
      };

      //! Convenience alias for ImageInfoTypeGenerator::type
      template <class T>
      using ImageInfoTypeGenerator_t = ImageInfoTypeGenerator<T>::type;
    }  // namespace

    //! Convenience alias for boost::gil backend types
    using ImageInfoType = ImageInfoTypeGenerator_t<ImageTags>;

    /**
     * @brief Class for storing and querying image metadata
     */
    class ImageInfo {
    public:
      /**
       * @brief Construct an ImageInfo object
       * @param info A variant containing the boost::gil backend to initialize
       * from
       */
      ImageInfo(const ImageInfoType& info);

      /**
       * @brief Construct an ImageInfo object
       * @param path The file to initialize from
       * @param tags A variant containing the image type tag
       */
      ImageInfo(const std::filesystem::path& path, const ImageTags& tags);

      /**
       * @brief Get the height of the image in pixels
       * @return The height of the image in pixels
       */
      std::uint32_t height() const;

      /**
       * @brief Get the width of the image in pixels
       * @return The width of the image in pixels
       */
      std::uint32_t width() const;

      /**
       * @brief Get the type of the image
       * @return A variant containing a boost::gil image tag type
       */
      ImageTags getType() const;

    private:
      /**
       * @brief Create a boost::gil backend from a file and image type info
       * @param path The path to the image
       * @param tags A variant containing the image tag type
       * @return A boost::gil reader backend for the given image
       */
      static ImageInfoType getImageInfo(const std::filesystem::path& path,
                                        const ImageTags& tags);

      //! The boost::gil reader backend
      ImageInfoType info;
    };

    /**
     * @brief Get the image file type by reading the file header
     * @param path The path to the image
     * @return An optional holding a variant containing the image tag type if
     * successful. A nullopt otherwise.
     */
    std::optional<ImageTags> getImageType(const std::filesystem::path& path);

    /**
     * @brief Scale an image's dimensions in pixels based on the desired height
     * in pixels
     * @param srcHeight The height to scale to. The maximum resulting height
     * from scaling.
     * @param info ImageInfo for the target image
     * @return A pair containing the height and width of the scaled target image
     * in pixels
     */
    std::pair<std::uint32_t, std::uint32_t> getScaledDimsFromHeight(
        std::uint32_t srcHeight, const ImageInfo& info);

    /**
     * @brief Determine regions of interest (ROIs) for a list of images to be
     * copied to the source image
     * @tparam View The source image view type
     * @tparam RangeOfImageInfo The type of the range of image metadata
     * @param src A view of the source image
     * @param info A range of image metadata used to determine ROIs
     * @return A range containing ROIs as pairs of points which define
     * rectangles using [top left, bottom right]. Returned ROIs are relative to
     * the source image. The actual point type is determined by the source image
     */
    template <class View, class RangeOfImageInfo>
    auto determineRoisFromImageInfo(const View& src,
                                    const RangeOfImageInfo& info) {
      std::uint32_t widthOfAllPx = 0;

      std::vector<std::pair<std::uint32_t, std::uint32_t>> scaledView;
      for (const auto& i : info) {
        auto scaled = getScaledDimsFromHeight(
            static_cast<std::uint32_t>(src.height()), i);
        auto checkedWidth = widthOfAllPx + scaled.first;
        if (checkedWidth > src.width()) {
          break;
        }
        widthOfAllPx = checkedWidth;
        scaledView.emplace_back(std::move(scaled));
      }

      const std::uint32_t widthSpacingPx =
          (static_cast<std::uint32_t>(src.width()) - widthOfAllPx) /
          static_cast<std::uint32_t>(scaledView.size() +
                                     1);  //+1 for outside edge

      using point_t = typename View::point_t;
      using RoiType = std::pair<point_t, point_t>;
      std::vector<RoiType> rois;
      std::uint32_t widthSoFar = 0;

      for (const auto& dims : scaledView) {
        widthSoFar += widthSpacingPx;
        std::uint32_t heightDiffPx = 0;
        if (dims.second < src.height()) {
          // center image in y dimension
          heightDiffPx =
              (static_cast<std::uint32_t>(src.height()) - dims.second) / 2;
        }
        rois.emplace_back(
            point_t{widthSoFar, heightDiffPx},
            point_t{dims.first + widthSoFar, dims.second + heightDiffPx});
        widthSoFar += dims.first;
      }
      return rois;
    }
  }  // namespace wp
}  // namespace brilliant