target_sources(${PROJECT_NAME}_ARCHIVE PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/ComLib.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/WallpaperSetterImpl.cpp"
)

target_include_directories(${PROJECT_NAME}_ARCHIVE PRIVATE BEFORE
	"${CMAKE_CURRENT_SOURCE_DIR}/Win"
)

target_compile_definitions(${PROJECT_NAME}_ARCHIVE PRIVATE 
	WINDOWS_LEAN_AND_MEAN
	NOMINMAX
	_WIN32_WINNT=0x0A00
)