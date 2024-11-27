/**
 *
 *  @file      ComLib.hpp
 *  @author    David Brill
 *  @copyright © David Brill, 2024. All right reserved.
 *
 *  Defines the ComLib class
 */
#pragma once

namespace brilliant {
  namespace wp {
    /**
     * @brief RAII wrapper for Windows Com utility
     *
     */
    class ComLib {
    public:
      /**
       * @brief Construct a ComLib object
       */
      ComLib();

      /**
       * @brief Destroy a ComLib object
       */
      ~ComLib();
    };
  }  // namespace wp
}  // namespace brilliant