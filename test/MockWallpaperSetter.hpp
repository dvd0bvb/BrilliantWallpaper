/**
 *
 *  @file      MockWallpaperSetter.hpp
 *  @author    David Brill
 *  @copyright © David Brill, 2024. All right reserved.
 *
 *  Defines the MockWallpaperSetter class
 */

class MockWallpaperSetterImpl {
public:
  MOCK_METHOD(void, setWallpaper,
              (std::uint32_t, const std::filesystem::path&));
  MOCK_METHOD((std::pair<std::uint32_t, std::uint32_t>), getResolution,
              (std::uint32_t));
};