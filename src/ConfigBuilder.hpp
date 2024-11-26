#pragma once

#include <filesystem>
#include <iosfwd>
#include "Config.hpp"

namespace brilliant {
  namespace wp {

    class ConfigBuilder
    {
    public:
      virtual Config build(const std::filesystem::path& path) = 0;

      virtual Config build(std::istream& is) = 0;
    };

  }
}