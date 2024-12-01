/**
 *
 *  @file      App.cpp
 *  @author    David Brill
 *  @copyright © David Brill, 2024. All right reserved.
 *
 *  Implements the App class
 */
#include "App.hpp"

#include <boost/asio.hpp>
#include <boost/gil.hpp>
#include <boost/gil/extension/numeric/resample.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <filesystem>
#include <ranges>
#include <string_view>

#include "GetInstallPath.hpp"
#include "Log.hpp"
#include "TomlConfigBuilder.hpp"

namespace brilliant {
  namespace wp {

    using namespace std::string_view_literals;

    namespace asio = boost::asio;

    //! Format for generated wallpaper file names
    constexpr auto fileNameFormat =
        "brilliant_wallpaper_m{}_{:%Y%m%d-%H%M%S}.jpg"sv;

    App::App(int argc, const char* argv[])
        : mt(rd()),
          tempDirectory(std::filesystem::temp_directory_path() /
                        "brilliant_wp") {
      // TODO: parse args
      //  - seed - seed the random number gen
      //  - configfile path - alternative path to the config file
      //  - tmp dir - alternative path to tmp directory
      //  - debug level
      installDirectory = getInstallPath() / "brilliant_wp";
      log(severity_level::debug, "Install directory: {}",
          installDirectory.string());

      TomlConfigBuilder builder;
      config = builder.build(installDirectory / "config.toml");

      if (!std::filesystem::exists(tempDirectory)) {
        std::filesystem::create_directory(tempDirectory);
        log(severity_level::debug, "Created temp directory: {}",
            tempDirectory.string());
      }
    }

    void App::run() {
      asio::thread_pool pool;
      timers.reserve(config.monitors.size());

      for (auto& [i, monitor] : config.monitors) {
        for (auto iter = monitor.backgroundPaths.begin();
             iter != monitor.backgroundPaths.end();) {
          if (auto tags = getImageType(*iter)) {
            fileInfoCache.emplace(*iter, ImageInfo{*iter, *tags});
            ++iter;
          } else {
            log(severity_level::warning,
                "The file type of {} could not be determined and so will not "
                "be included in source images",
                iter->string());
            iter = monitor.backgroundPaths.erase(iter);
          }
        }
      }

      for (auto i : config.monitors | std::views::keys) {
        auto& timer = timers.emplace_back(pool);
        asio::post(pool, [this, &timer, i] {
          try {
            // immediately make the first one
            auto wallpaper = makeNextWallpaper(i);
            const auto name = std::format(fileNameFormat, i,
                                          std::chrono::system_clock::now());
            const auto outPath = tempDirectory / name;
            boost::gil::write_view(
                outPath, boost::gil::const_view(wallpaper),
                boost::gil::image_write_info<boost::gil::jpeg_tag>());
            setter.setWallpaper(i, outPath);

            // prep the next
            wallpaper = makeNextWallpaper(i);
            const auto nextName = std::format(fileNameFormat, i,
                                              std::chrono::system_clock::now());
            boost::gil::write_view(
                tempDirectory / nextName, boost::gil::const_view(wallpaper),
                boost::gil::image_write_info<boost::gil::jpeg_tag>());

            const auto delay = config.monitors[i].transitionDelay
                                   ? *config.monitors[i].transitionDelay
                                   : config.globalTransitionDelay;

            log(severity_level::debug,
                "Next wallpaper for monitor {} saved to {}", i,
                (tempDirectory / nextName).string());
            log(severity_level::debug, "Next wallpaper will be set in {}",
                delay);

            // set async timer
            timer.expires_after(delay);
            timer.async_wait([this, &timer, nextName, i](const auto& ec) {
              onTimerExpiry(timer, i, nextName, ec);
            });
          } catch (const std::exception& e) {
            if (!eptr) {
              eptr = std::make_exception_ptr(e);
              stop();
            }
          }
        });
      }

      pool.join();

      if (eptr) {
        std::rethrow_exception(eptr);
      }
    }

    ImageType App::makeNextWallpaper(unsigned monitorIndex) {
      auto& monitor = config.monitors[monitorIndex];

      std::ranges::shuffle(monitor.backgroundPaths, mt);
      auto info = monitor.backgroundPaths |
                  std::views::transform([this](const auto& path) {
                    return fileInfoCache.at(path);
                  });

      const auto res = setter.getResolution(monitorIndex);
      boost::gil::rgb8_image_t combined(res.first, res.second);
      const auto rois = determineRoisFromImageInfo(combined, info);

      for (const auto& [path, roi] :
           std::views::zip(monitor.backgroundPaths, rois)) {
        const auto resizedWidth = roi.second.x - roi.first.x;
        const auto resizedHeight = roi.second.y - roi.first.y;

        ImageType rawImg;
        std::visit(
            [&](const auto& type) {
              boost::gil::read_image(path, rawImg, type);
            },
            fileInfoCache.at(path).getType());

        if (rawImg.height() != resizedHeight ||
            rawImg.width() != resizedWidth) {
          boost::gil::rgb8_image_t resized(resizedWidth, resizedHeight);
          boost::gil::resize_view(boost::gil::const_view(rawImg),
                                  boost::gil::view(resized),
                                  boost::gil::bilinear_sampler());
          boost::gil::copy_and_convert_pixels(
              boost::gil::const_view(resized),
              boost::gil::subimage_view(boost::gil::view(combined), roi.first.x,
                                        roi.first.y, resizedWidth,
                                        resizedHeight));
        } else {
          boost::gil::copy_and_convert_pixels(
              boost::gil::const_view(rawImg),
              boost::gil::subimage_view(boost::gil::view(combined), roi.first.x,
                                        roi.first.y, resizedWidth,
                                        resizedHeight));
        }
      }

      return combined;
    }

    void App::stop() {
      for (auto& timer : timers) {
        timer.cancel();
      }
    }

    void App::onTimerExpiry(boost::asio::steady_timer& timer,
                            unsigned monitorIndex, const std::string& name,
                            const boost::system::error_code& ec) {
      try {
        if (ec) {
          if (ec == asio::error::operation_aborted) {
            log(severity_level::info,
                "Async operation cancelled for monitor {}", monitorIndex);
            return;
          } else {
            asio::detail::throw_error(ec);
          }
        }

        const auto currentWallpaperPath = tempDirectory / name;
        setter.setWallpaper(monitorIndex, currentWallpaperPath);

        auto next = makeNextWallpaper(monitorIndex);
        const auto now = std::chrono::system_clock::now();
        const auto nextName = std::vformat(
            fileNameFormat, std::make_format_args(monitorIndex, now));

        // Remove old files
        for (const auto& entry :
             std::filesystem::directory_iterator(tempDirectory)) {
          if (entry.path() != currentWallpaperPath) {
            log(severity_level::debug, "Removing item: {}",
                entry.path().string());
            std::filesystem::remove_all(entry);
          }
        }

        boost::gil::write_view(
            tempDirectory / nextName, boost::gil::const_view(next),
            boost::gil::image_write_info<boost::gil::jpeg_tag>());
        const auto delay = config.monitors[monitorIndex].transitionDelay
                               ? *config.monitors[monitorIndex].transitionDelay
                               : config.globalTransitionDelay;

        log(severity_level::debug, "Next wallpaper for monitor {} saved to {}",
            monitorIndex, (tempDirectory / nextName).string());
        log(severity_level::debug, "Next wallpaper will be set in {}", delay);

        timer.expires_after(delay);
        timer.async_wait(
            [this, &timer, monitorIndex, nextName](const auto& ec) {
              onTimerExpiry(timer, monitorIndex, nextName, ec);
            });
      } catch (const std::exception& e) {
        if (!eptr) {
          eptr = std::make_exception_ptr(e);
          stop();
        }
      }
    }

  }  // namespace wp
}  // namespace brilliant