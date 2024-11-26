#include <comutil.h>
#include <comdef.h>
#include "WallpaperSetterImpl.hpp"
#include "../Log.hpp"

namespace brilliant {
  namespace wp {
    //TODO: create error codes/categories so these can throw instead of logging errors

    WallpaperSetterImpl::WallpaperSetterImpl() :
      manager([]() -> IDesktopWallpaper* {
        IDesktopWallpaper* ptr = nullptr;
        const auto result = CoCreateInstance(CLSID_DesktopWallpaper, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&ptr));
        CheckError(result);
        return ptr;
      }(), [](IDesktopWallpaper* ptr) {
        if (ptr)
        {
          ptr->Release();
        }
      })
    {

    }

    void WallpaperSetterImpl::setWallpaper(uint32_t monitorIndex, const std::filesystem::path& filepath)
    {
      auto monitorName = getMonitorName(monitorIndex);
      const auto result = manager->SetWallpaper(nullptr, filepath.native().c_str());
      CheckError(result);
      log(severity_level::info, "Successfully set wallpaper to {} on monitor {}", filepath.string(), monitorIndex);
    }

    std::pair<uint32_t, uint32_t> WallpaperSetterImpl::getResolution(uint32_t monitorIndex) 
    {
      auto monitorName = getMonitorName(monitorIndex);

      RECT rect{};
      const auto result = manager->GetMonitorRECT(monitorName.data(), &rect);
      CheckError(result);
      log(severity_level::debug, "GetMonitorRECT() returned ok. Rect: t {}, l {}, b {}, r {}", rect.top, rect.left, rect.bottom, rect.right);
      return { rect.right, rect.bottom };
    }

    std::wstring WallpaperSetterImpl::getMonitorName(uint32_t monitorIndex)
    {
      wchar_t* ptr = nullptr;
      const auto result = manager->GetMonitorDevicePathAt(monitorIndex, &ptr); //windows gives me hiv
      std::wstring monitorName;
      if (ptr)
      {
        monitorName.assign(ptr);
      }
      //need to call free even if the error check fails...
      ::CoTaskMemFree(ptr);
      CheckError(result);
      log(severity_level::debug, "GetMonitorDevicePath() returned ok. Monitor index: {}", monitorIndex);
      return monitorName;
    }

    void WallpaperSetterImpl::CheckError(HRESULT result)
    {
      try
      {
        _com_util::CheckError(result);
      }
      catch (const _com_error& e)
      {
        throw std::runtime_error(e.ErrorMessage());
      }
    }

  }
}