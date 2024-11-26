#pragma once

#include <string_view>
#include <format>
#include <boost/log/trivial.hpp>

namespace brilliant {
  namespace wp {

    using boost::log::trivial::severity_level;

    template<class... Args>
    void log(severity_level level, std::string_view format_string, Args&&... args) {
      BOOST_LOG_WITH_PARAMS(boost::log::trivial::logger::get(), (::boost::log::keywords::severity = level)) << std::vformat(format_string, std::make_format_args(const_cast<const Args&>(args)...));
    }

  }
}