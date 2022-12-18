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

find_path(BROTLI_ENCODE_INCLUDE_DIR brotli/encode.h REQUIRED)
find_path(BROTLI_DECODE_INCLUDE_DIR brotli/decode.h REQUIRED)
set(BROTLI_INCLUDE_DIR ${BROTLI_ENCODE_INCLUDE_DIR} ${BROTLI_DECODE_INCLUDE_DIR})
find_library(BROTLI_COMMON_LIB brotlicommon REQUIRED)
find_library(BROTLI_ENC_LIB brotlienc REQUIRED)
find_library(BROTLI_DEC_LIB brotlidec REQUIRED)
set(BROTLI_LIB ${BROTLI_COMMON_LIB} ${BROTLI_ENC_LIB} ${BROTLI_DEC_LIB})

get_property(ImportedTargets DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY IMPORTED_TARGETS)
message("Imported targets: ${ImportedTargets}")
message("List of compile features: ${CMAKE_CXX_COMPILE_FEATURES}")

add_subdirectory(src)
add_subdirectory(test)
