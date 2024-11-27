/**
 *
 *  @file      ConfigBuilder.hpp
 *  @author    David Brill
 *  @copyright © David Brill, 2024. All right reserved.
 *
 *  Defines the ConfigBuilder base class
 */
#pragma once

#include <filesystem>
#include <iosfwd>

#include "Config.hpp"

namespace brilliant {
  namespace wp {

    /**
     * @brief Base class for classes which read config data into memory
     */
    class ConfigBuilder {
    public:
      /**
       * @brief Read the config file at the given path
       * @param path The path to the config file
       * @return A Config object containing config data from the file
       */
      virtual Config build(const std::filesystem::path& path) = 0;

      /**
       * @brief Read config from an istream
       * @param is The istream to read from
       * @return A Config object containing config data form the istream
       */
      virtual Config build(std::istream& is) = 0;
    };

  }  // namespace wp
}  // namespace brilliant