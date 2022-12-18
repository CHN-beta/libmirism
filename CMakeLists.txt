cmake_minimum_required(VERSION 3.14)
project(libmirism)
enable_testing()
include(GNUInstallDirs)

find_package(fmt REQUIRED)

find_package(magic_enum REQUIRED)

find_package(cppcoro REQUIRED)

find_package(nameof REQUIRED)

find_path(TGBOTCPP_INCLUDE_DIR tgbot/tgbot.h REQUIRED)
find_library(TGBOTCPP_LIB libTgBot.so REQUIRED)

find_package(Boost REQUIRED COMPONENTS headers iostreams)

find_path(BACKTRACE_INCLUDE_DIR backtrace.h REQUIRED)
find_library(BACKTRACE_LIB backtrace REQUIRED)

get_property(ImportedTargets DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY IMPORTED_TARGETS)
message("Imported targets: ${ImportedTargets}")
message("List of compile features: ${CMAKE_CXX_COMPILE_FEATURES}")

add_library(mirism_utility SHARED src/detail_/misc/common.cpp src/detail_/misc/string.cpp src/detail_/misc/logger.cpp)
add_library(mirism::utility ALIAS mirism_utility)
target_include_directories(mirism_utility PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${TGBOTCPP_INCLUDE_DIR}
	${BACKTRACE_INCLUDE_DIR})
target_link_libraries(mirism_utility PRIVATE fmt::fmt magic_enum::magic_enum cppcoro::cppcoro nameof::nameof
	${TGBOTCPP_LIB} Boost::headers Boost::iostreams ${BACKTRACE_LIB})
set_property(TARGET mirism_utility PROPERTY CXX_STANDARD 23)
set_property(TARGET mirism_utility PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET mirism_utility PROPERTY CXX_EXTENSIONS OFF)
