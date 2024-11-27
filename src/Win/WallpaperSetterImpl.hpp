/**
 *
 *  @file      WallpaperSetterImpl.hpp
 *  @author    David Brill
 *  @copyright © David Brill, 2024. All right reserved.
 *
 *  Defines the WallpaperSetterImpl class
 */
#pragma once

#include <ShObjIdl.h>
#include <windows.h>

#include <filesystem>
#include <memory>

#include "ComLib.hpp"

namespace brilliant {
  namespace wp {
    /**
     * @brief Windows specific implementation of a WallpaperSetter
     *
     */
    class WallpaperSetterImpl {
    public:
      /**
       * @brief Construct a WallpaperSetterImpl
       *
       */
      WallpaperSetterImpl();

      /**
       * @brief Set the wallpaper of the given monitor to the image at the given
       * location
       * @param monitorIndex The index of the monitor to set
       * @param filepath Path to the image file to set the wallpaper to
       */
      void setWallpaper(uint32_t monitorIndex,
                        const std::filesystem::path& filepath);

      /**
       * @brief Get the resolution of a monitor
       * @param monitorIndex The index of the monitor
       * @return A pair containing the width and height of the monitor in pixels
       */
      std::pair<uint32_t, uint32_t> getResolution(uint32_t monitorIndex);

      /**
       * @brief Get the name of a monitor
       * @param monitorIndex The index of the monitor
       * @return The monitor name as a wstring
       */
      std::wstring getMonitorName(uint32_t monitorIndex);

      /**
       * @brief Check an HRESULT for an error code
       * @param result The value to check
       * @throw std::runtime_error if the
       */
      static void CheckError(HRESULT result);

    private:
      //! The Com object which needs to exist to use Windows Com objects
      ComLib lib;

      //! An IDesktopWallpaper instance used to query and change the wallpaper. From the Windows Com library
      std::unique_ptr<IDesktopWallpaper, void (*)(IDesktopWallpaper*)> manager;
    };

  }  // namespace wp
}  // namespace brilliant