#pragma once

#include <memory>
#include <filesystem>
#include <utility>

namespace brilliant {
  namespace wp {

    class WallpaperSetterImpl;

    class WallpaperSetter
    {
    public:
      WallpaperSetter();

      ~WallpaperSetter();

      void setWallpaper(std::uint32_t monitorIndex, const std::filesystem::path& filepath);

      std::pair<std::uint32_t, std::uint32_t> getResolution(std::uint32_t monitorIndex);

    private:
      std::unique_ptr<WallpaperSetterImpl> _impl;
    };
  }
}