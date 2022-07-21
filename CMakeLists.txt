cmake_minimum_required(VERSION 3.14)
project(libmirism)
enable_testing()
include(GNUInstallDirs)

find_package(fmt REQUIRED)

find_path(NAMEOF_INCLUDE_DIR nameof.hpp REQUIRED)

find_package(Boost REQUIRED)

find_path(BACKTRACE_INCLUDE_DIR backtrace.h REQUIRED)
find_library(BACKTRACE_LIB backtrace REQUIRED)

find_path(TGBOTCPP_INCLUDE_DIR tgbot/tgbot.h REQUIRED)
find_library(TGBOTCPP_LIB libTgBot.so REQUIRED)

find_package(magic_enum REQUIRED)

add_library(libmirism INTERFACE)
target_include_directories(libmirism INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include ${NAMEOF_INCLUDE_DIR}
	${BACKTRACE_INCLUDE_DIR} ${TGBOTCPP_INCLUDE_DIR})
target_link_libraries(libmirism INTERFACE fmt::fmt Boost::boost ${BACKTRACE_LIB} ${TGBOTCPP_LIB} magic_enum::magic_enum)
target_compile_features(libmirism INTERFACE cxx_std_23)
target_compile_options(libmirism INTERFACE -Wno-pmf-conversions)

option(MIRISM_ENABLE_HTTPLIB "Enable httplib backend" OFF)
if(MIRISM_ENABLE_HTTPLIB)
	find_package(httplib REQUIRED)
	target_link_libraries(libmirism INTERFACE httplib::httplib)
endif()

add_subdirectory(test)
