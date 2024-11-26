#pragma once

#include <memory>
#include <filesystem>

#include <windows.h>
#include <ShObjIdl.h>

#include "ComLib.hpp"

namespace brilliant {
  namespace wp {

    class WallpaperSetterImpl
    {
    public:
      
      WallpaperSetterImpl();

      void setWallpaper(uint32_t monitorIndex, const std::filesystem::path& filepath);

      std::pair<uint32_t, uint32_t> getResolution(uint32_t monitorIndex);

      std::wstring getMonitorName(uint32_t monitorIndex);

      static void CheckError(HRESULT result);

    private:
      ComLib lib;

      std::unique_ptr<IDesktopWallpaper, void(*)(IDesktopWallpaper*)> manager;
    };

  }
}