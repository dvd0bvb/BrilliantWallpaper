/**
 *
 *  @file      Log.hpp
 *  @author    David Brill
 *  @copyright © David Brill, 2024. All right reserved.
 *
 *  Implement the log function template
 */

#pragma once

#include <format>
#include <string_view>

#include <boost/log/trivial.hpp>

namespace brilliant {
  namespace wp {

    using boost::log::trivial::severity_level;

    /**
     * @brief Log a message
     * @tparam ...Args The argument(s) type(s)
     * @param level The log level
     * @param format_string The format string. Uses std::format formatting style
     * @param ...args The argument(s) to format
     */
    template <class... Args>
    void log(severity_level level, std::string_view format_string,
             Args&&... args) {
      BOOST_LOG_WITH_PARAMS(boost::log::trivial::logger::get(),
                            (::boost::log::keywords::severity = level))
          << std::vformat(format_string, std::make_format_args(
                                             const_cast<const Args&>(args)...));
    }

  }  // namespace wp
}  // namespace brilliant