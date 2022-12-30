cmake_minimum_required(VERSION 3.14)
project(libmirism VERSION 1.0.0 LANGUAGES CXX)
enable_testing()
include(GNUInstallDirs)

# if cxx compiler is not gcc-12, warn user
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR NOT CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "12.0.0")
	message(WARNING "C++ compiler is not gcc-12.0.0 or higher. Some features may not work.")
endif()

install(EXPORT MirismConfig DESTINATION share/Mirism/cmake)

add_subdirectory(src)
add_subdirectory(test)
