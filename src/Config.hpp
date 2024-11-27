/**
 *
 *  @file      Config.hpp
 *  @author    David Brill
 *  @copyright © David Brill, 2024. All right reserved.
 * 
 *  Implements data structures for config data
 */
#pragma once

#include <chrono>
#include <exception>
#include <filesystem>
#include <iosfwd>
#include <optional>
#include <unordered_map>
#include <vector>

namespace brilliant {
  namespace wp {

    /**
     * @brief Config specific exception type
     */
    struct ConfigError : std::runtime_error {
      using runtime_error::runtime_error;
    };

    /**
     * @brief Configuration data for a monitor
     */
    struct ConfigMonitor {
      //! Paths to images used for wallpapers
      std::vector<std::filesystem::path> backgroundPaths;

      //! The monitor specific transition delay
      std::optional<std::chrono::minutes> transitionDelay;

      //! The index of the monitor
      std::optional<std::uint32_t> index;
    };

    /**
     * @brief Container for configuration data
     */
    struct Config {
      //! The global transition delay
      std::chrono::minutes globalTransitionDelay;
      
      //! Storage for monitor specific config data
      std::unordered_map<std::uint32_t, ConfigMonitor> monitors;
    };
  }  // namespace wp
}  // namespace brilliant