#pragma once

#include <atomic>
#include <memory>
#include <vector>
#include <exception>
#include <random>
#include <filesystem>
#include <unordered_map>
#include <mutex>

#include <boost/asio/steady_timer.hpp>

#include "WallpaperSetter.hpp"
#include "Config.hpp"
#include "ImageProcessing.hpp"

namespace brilliant {
  namespace wp {

    class App
    {
    public:

      App(int argc, const char* argv[]);

      void run();

      ImageType makeNextWallpaper(unsigned monitorIndex);

      void stop();

      void onTimerExpiry(boost::asio::steady_timer& timer, unsigned monitorIndex, const std::string& name, const boost::system::error_code& ec);

    private:
      std::atomic_bool stopped;

      Config config;

      WallpaperSetter setter;

      std::vector<boost::asio::steady_timer> timers;

      std::exception_ptr eptr;

      std::random_device rd;

      std::mt19937 mt;

      std::filesystem::path tempDirectory;

      std::filesystem::path installDirectory;

      std::unordered_map<std::filesystem::path, ImageInfo> fileInfoCache;
    };

  }
}