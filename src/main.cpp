/**
 *
 *  @file      main.cpp
 *  @author    David Brill
 *  @copyright © David Brill, 2024. All right reserved.
 *
 *  Implements main
 */

// TODO remove this from here for cross compatibility
#include <sdkddkver.h>

#include "App.hpp"
#include "Log.hpp"

int main(int argc, const char* argv[]) {
  try {
    brilliant::wp::App app(argc, argv);
    app.run();
    return EXIT_SUCCESS;
  } catch (const std::exception& e) {
    brilliant::wp::log(brilliant::wp::severity_level::error,
                       "Caught exception: {}", e.what());
    return EXIT_FAILURE;
  }
}