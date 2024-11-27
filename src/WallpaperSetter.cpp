/**
 *
 *  @file      WallpaperSetter.cpp
 *  @author    David Brill
 *  @copyright © David Brill, 2024. All right reserved.
 *
 *  Implements the WallpaperSetter class
 */
#include "WallpaperSetter.hpp"

#ifdef WIN32
#include "Win/WallpaperSetterImpl.hpp"
#endif

namespace brilliant {
  namespace wp {

    WallpaperSetter::WallpaperSetter()
        : _impl(std::make_unique<WallpaperSetterImpl>()) {}

    WallpaperSetter::~WallpaperSetter() = default;

    void WallpaperSetter::setWallpaper(std::uint32_t monitorIndex,
                                       const std::filesystem::path& filepath) {
      _impl->setWallpaper(monitorIndex, filepath);
    }

    std::pair<std::uint32_t, std::uint32_t> WallpaperSetter::getResolution(
        std::uint32_t monitorIndex) {
      return _impl->getResolution(monitorIndex);
    }

  }  // namespace wp
}  // namespace brilliant