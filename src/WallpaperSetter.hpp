/**
 *
 *  @file      WallpaperSetter.hpp
 *  @author    David Brill
 *  @copyright © David Brill, 2024. All right reserved.
 *
 *  Defines the WallpaperSetter class
 */
#pragma once

#include <filesystem>
#include <memory>
#include <utility>

namespace brilliant {
  namespace wp {
    //! Forward declare the implementation type
    class WallpaperSetterImpl;

    /**
     * @brief Interface for interacting with the wallpaper
     */
    class WallpaperSetter {
    public:
      /**
       * @brief Construct a WallpaperSetter
       */
      WallpaperSetter();

      /**
       * @brief Destry a WallpaperSetter
       */
      ~WallpaperSetter();

      /**
       * @brief Set the wallpaper for the given monitor
       * @param monitorIndex The index of the monitor
       * @param filepath Path to the image file to set the wallpaper to
       */
      void setWallpaper(std::uint32_t monitorIndex,
                        const std::filesystem::path& filepath);

      /**
       * @brief Get the resolution of the given monitor in pixels
       * @param monitorIndex The index of the monitor to query
       * @return A pair containing the width and height of the monitor in pixels
       */
      std::pair<std::uint32_t, std::uint32_t> getResolution(
          std::uint32_t monitorIndex);

    private:
      //! A pointer to the wallpaper setter implementation object
      std::unique_ptr<WallpaperSetterImpl> _impl;
    };
  }  // namespace wp
}  // namespace brilliant