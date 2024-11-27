/**
 *
 *  @file      App.hpp
 *  @author    David Brill
 *  @copyright © David Brill, 2024. All right reserved.
 *
 *  Defines the App class
 */
#pragma once

#include <atomic>
#include <exception>
#include <filesystem>
#include <memory>
#include <mutex>
#include <random>
#include <unordered_map>
#include <vector>

#include <boost/asio/steady_timer.hpp>

#include "Config.hpp"
#include "ImageProcessing.hpp"
#include "WallpaperSetter.hpp"

namespace brilliant {
  namespace wp {
    /**
     * @brief Provides the basic functionality of the BrilliantWallpaper
     * application
     *
     * A standalone class was used to allow for easier exception handling across
     * threads and to separate app functionality into logical pieces. The app 
     * creates a thread pool to handle creation and setting of wallpapers. 
     * 
     * The first time the app is run, a wallpaper is generated for each configured 
     * monitor and the wallpaper is set when that is done. The next wallpaper is generated 
     * and stored and then a timer dedicated to that monitor is set. Once the timer
     * expires the previously generated wallpaper is set for that monitor and the process
     * begins again.
     */
    class App {
    public:
      /**
       * @brief Construct an App object
       * @param argc The number of arguments from the command line
       * @param argv Arguments from the command line
       */
      App(int argc, const char* argv[]);

      /**
       * @brief Run the app
       * 
       * Starts the thread pool and delegates building wallpapers to the pool threads.
       */
      void run();

      /**
       * @brief Make the next wallpaper
       * @param monitorIndex The monitor to generate a wallpaper for
       * @return The generated image
       */
      ImageType makeNextWallpaper(unsigned monitorIndex);

      /**
       * @brief Stop the thread pool
       */
      void stop();

      /**
       * @brief Callback to handle setting the wallpaper on timer expiry.
       * @param timer The timer object that is handling the callback
       * @param monitorIndex The index of the monitor that is having its wallpaper set
       * @param name The name of the file to set as the wallpaper
       * @param ec An error code passed from the timer
       */
      void onTimerExpiry(boost::asio::steady_timer& timer,
                         unsigned monitorIndex, const std::string& name,
                         const boost::system::error_code& ec);

    private:
      //! A flag to indicate the app has been stopped
      std::atomic_bool stopped;

      //! Config options
      Config config;

      //! The wallpaper setter object
      WallpaperSetter setter;

      //! Storage for timers handling callbacks 
      std::vector<boost::asio::steady_timer> timers;

      //! An exception pointer used to get exceptions across thread boundaries
      std::exception_ptr eptr;

      //! A random device
      std::random_device rd;

      //! A random number generator
      std::mt19937 mt;

      //! The temp directory. %TEMP%/briliant_wp on Windows and /tmp/brilliant_wp on Linux
      std::filesystem::path tempDirectory;

      //! The install directory, used to find the config file. %PROGRAM_FILES%/brilliant_wp on Windows, /usr/local/bin/brilliant_wp on Linux
      std::filesystem::path installDirectory;

      //! Map of image paths to their metadata
      std::unordered_map<std::filesystem::path, ImageInfo> fileInfoCache;
    };

  }  // namespace wp
}  // namespace brilliant