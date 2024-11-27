/**
 *
 *  @file      GetInstallPath.cpp
 *  @author    David Brill
 *  @copyright © David Brill, 2024. All right reserved.
 *
 *  Implements the getInstallPath function
 */
#include "GetInstallPath.hpp"
#ifdef WIN32
#include "Win/GetInstallPathImpl.hpp"
#endif

namespace brilliant {
  namespace wp {

    std::filesystem::path getInstallPath() { return getInstallPathImpl(); }

  }  // namespace wp
}  // namespace brilliant
