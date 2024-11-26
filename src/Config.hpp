#pragma once

#include <chrono>
#include <filesystem>
#include <iosfwd>
#include <optional>
#include <unordered_map>
#include <vector>
#include <exception>

namespace brilliant {
  namespace wp {

    struct ConfigError : std::runtime_error 
    {
      using runtime_error::runtime_error;
    };

    struct ConfigMonitor 
    {
      std::vector<std::filesystem::path> backgroundPaths;

      std::optional<std::chrono::minutes> transitionDelay;

      std::optional<std::uint32_t> index;
    };

    struct Config
    {
      std::chrono::minutes globalTransitionDelay;

      std::unordered_map<std::uint32_t, ConfigMonitor> monitors;
    };
  }
}