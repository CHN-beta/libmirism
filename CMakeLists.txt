cmake_minimum_required(VERSION 3.14)
project(libmirism)
enable_testing()
include(GNUInstallDirs)

find_package(fmt REQUIRED)
find_path(NAMEOF_INCLUDE_DIR nameof.hpp REQUIRED)
find_package(Boost REQUIRED COMPONENTS container)

add_library(libmirism INTERFACE)
target_include_directories(libmirism INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include ${NAMEOF_INCLUDE_DIR})
target_link_libraries(libmirism INTERFACE fmt::fmt Boost::container)
target_compile_features(libmirism INTERFACE cxx_std_23)

option(MIRISM_LOG_DEBUG "Enable debug message to be printed." OFF)
if(MIRISM_LOG_DEBUG)
	target_compile_definitions(libmirism INTERFACE MIRISM_LOG_DEBUG)
endif()

add_subdirectory(test)
