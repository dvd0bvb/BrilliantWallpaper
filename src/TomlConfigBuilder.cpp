#include <format>
#include <istream>
#include <string_view>
#include <ranges>
#include <sstream>
#include <functional>
#include <algorithm>

#include "TomlConfigBuilder.hpp"

template <>
struct std::formatter<toml::parse_error> {
  constexpr std::format_parse_context::iterator parse(std::format_parse_context& context) {
    auto it = context.begin();
    if (it == context.end()) {
      return it;
    }

    if (*it != '}') {
      throw std::format_error("Invalid format args for toml::parse_error.");
    }

    return it;
  }

  std::format_context::iterator format(const toml::parse_error& error, std::format_context& context) const {
    std::ostringstream ss;
    ss << error;
    return std::ranges::copy(std::move(ss).str(), context.out()).out;
  }
};

template <class T>
struct std::formatter<toml::node_view<T>> {
  constexpr std::format_parse_context::iterator parse(std::format_parse_context& context) {
    auto it = context.begin();
    if (it == context.end()) {
      return it;
    }

    if (*it != '}') {
      throw std::format_error("Invalid format args for toml::node_view.");
    }

    return it;
  }

  std::format_context::iterator format(const toml::node_view<T>& node, std::format_context& context) const {
    std::ostringstream ss;
    ss << node;
    return std::ranges::copy(std::move(ss).str(), context.out()).out;
  }
};

template <>
struct std::formatter<toml::node> {
  constexpr std::format_parse_context::iterator parse(std::format_parse_context& context) {
    auto it = context.begin();
    if (it == context.end()) {
      return it;
    }

    if (*it != '}') {
      throw std::format_error("Invalid format args for toml::node.");
    }

    return it;
  }

  std::format_context::iterator format(const toml::node& node, std::format_context& context) const {
    return std::format_to(context.out(), "{}", toml::node_view(node));
  }
};

namespace brilliant {
  namespace wp {

    using namespace std::string_view_literals;

    namespace keys {
      constexpr auto globalTransitionDelay = "transitionDelay"sv;

      constexpr auto monitors = "monitors"sv;

      namespace monitor {
        constexpr auto wallpapers = "wallpapers"sv;

        constexpr auto transitionDelay = "transitionDelay"sv;

        constexpr auto index = "index"sv;
      }
    }

    namespace defaults {
      constexpr auto globalTransitionMinutes = 30;
    }

    Config TomlConfigBuilder::build(const std::filesystem::path& path) {
      Config config{};
      const auto result = toml::parse_file(path.native());
      validate(result, path);
      parse(config, result);
      return config;
    }

    Config TomlConfigBuilder::build(std::istream& is) {
      Config config{};
      const auto result = toml::parse(is);
      validate(result, "istream");
      parse(config, result);
      return config;
    }

    void TomlConfigBuilder::validate(const toml::parse_result& result, const std::filesystem::path& configPath) {
      if (!result) {
        throw ConfigError(std::format("Failed to parse config file {}: {}", configPath.string(), result.error()));
      }

      auto& table = result.table();
      if (auto monitors = table.get(keys::monitors); monitors && monitors->is_array()) {
        if (monitors->as_array()->empty()) {
          throw ConfigError(std::format("The field {} has no entries: {}", keys::monitors, *monitors));
        }

        monitors->as_array()->for_each([](auto&& monitor) {
          if constexpr (toml::is_table<decltype(monitor)>) {
            if (auto wallpapers = monitor.get(keys::monitor::wallpapers); wallpapers && wallpapers->is_array()) {
              wallpapers->as_array()->for_each([](auto&& path) {
                if constexpr (!toml::is_string<decltype(path)>) {
                  throw ConfigError(std::format("Entry {} is not an array of strings: {}", keys::monitor::wallpapers, toml::node_view(path)));
                }
              });
            }
            else if (wallpapers) {
              throw ConfigError(std::format("Entry {} is not an array: {}", keys::monitor::wallpapers, *wallpapers));
            }
            else {
              throw ConfigError(std::format("Entry {}.{} is missing: {}", keys::monitors, keys::monitor::wallpapers, toml::node_view(monitor)));
            }

            if (auto delay = monitor.get(keys::monitor::transitionDelay); delay && !delay->is_number()) {
              throw ConfigError(std::format("Entry {}.{} is not a number: {}", keys::monitors, keys::monitor::transitionDelay, *delay));
            }

            if (auto index = monitor.get(keys::monitor::index); index && !index->is_number()) {
              throw ConfigError(std::format("Entry {}.{} is not a number: {}", keys::monitors, keys::monitor::index, *index));
            }
          }
          else {
            throw ConfigError(std::format("Found {} entry that is not a table: {}", keys::monitors, toml::node_view(monitor)));
          }
        });
      }
      else if (monitors) {
        throw ConfigError(std::format("The field {} is not an array: {}", keys::monitors, *monitors));
      }
      else {
        throw ConfigError(std::format("The field {} is missing", keys::monitors));
      }
    }

    void TomlConfigBuilder::parse(Config& config, const toml::table& table) {
      if (auto transDelay = table[keys::globalTransitionDelay].value<std::int64_t>()) {
        config.globalTransitionDelay = std::chrono::minutes(*transDelay);
      }
      else {
        config.globalTransitionDelay = std::chrono::minutes(defaults::globalTransitionMinutes);
      }

      const auto monitors = table[keys::monitors].as_array();
      auto configMonitors = *monitors | std::views::transform([this](auto&& table) {
        return parseMonitor(*table.as_table());
      });

      auto hasIndex = [](const auto& configMonitor) {
        return configMonitor.index.has_value();
      };

      for (const auto& configMonitor : configMonitors | std::views::filter(hasIndex))
      {
        config.monitors.emplace(*configMonitor.index, configMonitor);
      }

      std::uint32_t min = 0u;
      if (!config.monitors.empty()) {
        const auto keys = config.monitors | std::views::keys;
        min = std::min(*std::ranges::min_element(keys), min);
      }
      auto notIndexed = configMonitors | std::views::filter(std::not_fn(hasIndex));
      auto unusedIndexes = std::views::iota(min) | std::views::filter([&config](auto i) {
        return !config.monitors.count(i);
      });

      for (const auto& [i, configMonitor] : std::views::zip(unusedIndexes, notIndexed))
      {
        config.monitors.emplace(i, configMonitor);
      }
    }

    ConfigMonitor TomlConfigBuilder::parseMonitor(const toml::table& table) {
      ConfigMonitor configMonitor;

      for (const auto& path : *table[keys::monitor::wallpapers].as_array() | std::views::transform([](auto&& path) {
             return std::filesystem::path(*path.value<std::string_view>());
           })) {
        if (std::filesystem::is_directory(path)) {
          configMonitor.backgroundPaths.append_range(std::filesystem::recursive_directory_iterator(path));
        }
        else {
          configMonitor.backgroundPaths.push_back(path);
        }
      }

      if (const auto value = table[keys::monitor::transitionDelay].value<std::int64_t>()) {
        configMonitor.transitionDelay = std::chrono::minutes(*value);
      }

      if (const auto index = table[keys::monitor::index].value<std::int64_t>()) {
        configMonitor.index.emplace(static_cast<std::uint32_t>(*index));
      }
      return configMonitor;
    }
  }
}