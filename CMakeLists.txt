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

add_library(libmirism INTERFACE)
target_include_directories(libmirism INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include ${NAMEOF_INCLUDE_DIR}
	${BACKTRACE_INCLUDE_DIR} ${TGBOTCPP_INCLUDE_DIR})
target_link_libraries(libmirism INTERFACE fmt::fmt Boost::boost ${BACKTRACE_LIB} ${TGBOTCPP_LIB})
target_compile_features(libmirism INTERFACE cxx_std_23)

add_subdirectory(test)
