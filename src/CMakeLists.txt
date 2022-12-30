option(BUILD_UTILITY "Build utility library" ON)
option(BUILD_FRAMEWORK "Build framework library" ON)

if (BUILD_UTILITY)
	add_subdirectory(utility)
endif()
if (BUILD_FRAMEWORK)
	add_subdirectory(framework)
endif()
