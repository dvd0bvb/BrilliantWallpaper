include(${CMAKE_SOURCE_DIR}/cmake/CompilerOptions.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SanitizerOptions.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/MsvcRuntime.cmake)

set(MAIN_TARGET_SOURCES App.cpp GetInstallPath.cpp
  ImageProcessing.cpp main.cpp TomlConfigBuilder.cpp 
  WallpaperSetter.cpp
)

if(BRILLIANT_CMAKE_BUILD_EXECUTABLE)
  add_executable(${PROJECT_NAME} ${MAIN_TARGET_SOURCES})
elseif(BRILLIANT_CMAKE_BUILD_SHARED)
  add_library(${PROJECT_NAME} SHARED ${MAIN_TARGET_SOURCES})
  set_target_properties(
    ${PROJECT_NAME} PROPERTIES VERSION ${CMAKE_PROJECT_VERSION}
                               SOVERSION ${CMAKE_PROJECT_VERSION_MAJOR}
  )
else()
  add_library(${PROJECT_NAME} ${MAIN_TARGET_SOURCES})
endif()

if(WIN32)
  # windows specific stuff
  add_subdirectory(Win)
# TODO: add subdirs for linux specific things. Someday...
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_compile_features(${PROJECT_NAME} PRIVATE ${THIS_CXX_VERSION})

set_compiler_flags(${PROJECT_NAME} PRIVATE)
set_sanitizer_options(${PROJECT_NAME})
set_msvc_runtime(${PROJECT_NAME})

# third pary components
find_package(Boost REQUIRED 
  COMPONENTS log log_setup headers
)
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})

find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(libjpeg-turbo REQUIRED)
find_package(tomlplusplus REQUIRED)

target_link_libraries( ${PROJECT_NAME} PRIVATE 
  Boost::log_setup
  Boost::log 
  Boost::boost
  libjpeg-turbo::jpeg
  ZLIB::ZLIB 
  PNG::PNG
  tomlplusplus::tomlplusplus
)