#include "GetInstallPath.hpp"
#ifdef WIN32
#include "Win/GetInstallPathImpl.hpp"
#endif 

namespace brilliant {
  namespace wp {

    std::filesystem::path getInstallPath() {
      return getInstallPathImpl();
    }

  }
}
