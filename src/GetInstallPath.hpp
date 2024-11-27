/**
 *
 *  @file      GetInstallPath.hpp
 *  @author    David Brill
 *  @copyright © David Brill, 2024. All right reserved.
 *
 *  Define the getInstallPath function
 */
#pragma once

#include <filesystem>

namespace brilliant {
  namespace wp {

    /**
     * @brief Get the default install path
     * @return The path to the instll directory
     *
     * This will be generally %PROGRAM_FILES% on Windows and /usr/local/bin on
     * Linux
     */
    std::filesystem::path getInstallPath();

  }  // namespace wp
}  // namespace brilliant