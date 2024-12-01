include(${CMAKE_SOURCE_DIR}/cmake/CompilerOptions.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SanitizerOptions.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/MsvcRuntime.cmake)

set(TEST_TARGET ${PROJECT_NAME}_TEST)

set(TEST_SOURCES
  TestTomlConfigBuilder.cpp
  TestImageProcessing.cpp
)

set(TEST_DEPENDENCIES ${PROJECT_NAME}_ARCHIVE)

# copy files used for unit tests to bin directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/files DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${TEST_TARGET} ${TEST_SOURCES})

target_include_directories(${TEST_TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src
)

target_compile_features(${TEST_TARGET} PRIVATE ${THIS_CXX_VERSION})

set_compiler_flags(${TEST_TARGET} PRIVATE)
set_sanitizer_options(${TEST_TARGET})
set_msvc_runtime(${TEST_TARGET})

find_package(GTest REQUIRED)

target_link_libraries(
  ${TEST_TARGET} PRIVATE GTest::gmock GTest::gtest 
	# use custom main() for msvc since tests are not added correctly with gtest_discover_tests()
	$<$<NOT:${MSVC}>:GTest::gtest_main> 
	${TEST_DEPENDENCIES} 
)

if (${MSVC})
  add_subdirectory(Win)
else()
  # TODO: add test directory for not windows
endif()

include(GoogleTest)
gtest_discover_tests(${TEST_TARGET})
