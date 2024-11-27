/**
 *
 *  @file      GetInstallPathImpl.hpp
 *  @author    David Brill
 *  @copyright © David Brill, 2024. All right reserved.
 *
 *  Implements the getInstallPathImpl function 
 */
#pragma once 

#include <filesystem>
#include <system_error>
#include <ShlObj_core.h>

namespace brilliant {
  namespace wp {

    /**
     * @brief Get the Windows specific install path 
     */
    inline std::filesystem::path getInstallPathImpl() {
      PWSTR pathPtr = nullptr;
      const auto result = SHGetKnownFolderPath(FOLDERID_ProgramFiles, 0, 0, &pathPtr);
      std::filesystem::path path(pathPtr);
      CoTaskMemFree(pathPtr);
      if (std::error_code ec{ result, std::system_category() })
      {
        throw std::system_error(ec, "Failed to get install directory");
      }
      return path;
    }

  }
}