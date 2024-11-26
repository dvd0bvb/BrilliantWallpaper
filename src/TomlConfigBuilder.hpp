#pragma once

//we don't want toml++ "leaking" out of this file
//allows us to change config implementation/format later if desired
#ifndef TOML_EXCEPTIONS
#define TOML_EXCEPTIONS 0
#endif
#include <toml++/toml.h>

#include "ConfigBuilder.hpp"

namespace brilliant {
  namespace wp {

    class TomlConfigBuilder : public ConfigBuilder
    {
    public:
      Config build(const std::filesystem::path& path) override;

      Config build(std::istream& is) override;

      void validate(const toml::parse_result& result, const std::filesystem::path& configPath);

      void parse(Config& config, const toml::table& table);

      ConfigMonitor parseMonitor(const toml::table& table);
    };

  }
}