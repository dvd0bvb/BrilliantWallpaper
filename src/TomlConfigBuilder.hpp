/**
 *
 *  @file      TomlConfigBuilder.hpp
 *  @author    David Brill
 *  @copyright © David Brill, 2024. All right reserved.
 *
 *  Defines the TomlConfigBuilder class
 */
#pragma once

// we don't want toml++ "leaking" out of this file
// allows us to change config implementation/format later if desired
#ifndef TOML_EXCEPTIONS
#define TOML_EXCEPTIONS 0
#endif  // TOML_EXCEPTIONS
#include <toml++/toml.h>

#include "ConfigBuilder.hpp"

namespace brilliant {
  namespace wp {
    /**
     * @brief An implementation of ConfigBuilder which reads config in the toml
     * format
     */
    class TomlConfigBuilder : public ConfigBuilder {
    public:
      /**
       * @brief Build config data from the given file
       * @param path The path to the file to read config from
       * @return A Config object holding config data from the file
       * @throws ConfigError if validation or parsing fail
       */
      Config build(const std::filesystem::path& path) override;

      /**
       * @brief Build config data from the given istream
       * @param is The istream
       * @return A Config object holding config data from the istream
       * @throws ConfigError if validation or parsing fail
       */
      Config build(std::istream& is) override;

      /**
       * @brief Check the result of reading toml data and that all necessary
       * config data is present
       * @param result The result of reading data from toml++
       * @param configPath The config data file path. Used for generating error
       * messages.
       */
      void validate(const toml::parse_result& result,
                    const std::filesystem::path& configPath);

      /**
       * @brief Parse the table created from toml++ into a Config object
       * @param config A reference to the Config object to be modified
       * @param table The table created from toml++ parsing
       */
      void parse(Config& config, const toml::table& table);

      /**
       * @brief Parse monitor specific data
       * @param table The toml++ table to parse
       * @return A ConfigMontior object containing monitor specific config data
       */
      ConfigMonitor parseMonitor(const toml::table& table);
    };

  }  // namespace wp
}  // namespace brilliant