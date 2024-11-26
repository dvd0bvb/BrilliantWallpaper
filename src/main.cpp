//TODO remove this from here for cross compatibility
#include <sdkddkver.h>

#include "App.hpp"
#include "Log.hpp"

/*
needs:
- async accept signals to change log level - in windows this is messages
*/

int main(int argc, const char* argv[])
{
  try
  {
    brilliant::wp::App app(argc, argv);
    app.run();
    return EXIT_SUCCESS;
  }
  catch (const std::exception& e)
  {
    brilliant::wp::log(brilliant::wp::severity_level::error, "Caught exception: {}", e.what());
    return EXIT_FAILURE;
  }
}