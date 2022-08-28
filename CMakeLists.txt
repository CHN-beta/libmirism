cmake_minimum_required(VERSION 3.14)
project(libmirism)
enable_testing()
include(GNUInstallDirs)

find_package(fmt 8 REQUIRED)

find_package(nameof REQUIRED)

find_package(Boost REQUIRED COMPONENTS headers iostreams)

find_path(BACKTRACE_INCLUDE_DIR backtrace.h REQUIRED)
find_library(BACKTRACE_LIB backtrace REQUIRED)

find_path(TGBOTCPP_INCLUDE_DIR tgbot/tgbot.h REQUIRED)
find_library(TGBOTCPP_LIB libTgBot.so REQUIRED)

find_package(magic_enum REQUIRED)

find_package(OpenSSL REQUIRED)

find_path(BROTLI_ENCODE_INCLUDE_DIR brotli/encode.h REQUIRED)
find_path(BROTLI_DECODE_INCLUDE_DIR brotli/decode.h REQUIRED)
set(BROTLI_INCLUDE_DIR ${BROTLI_ENCODE_INCLUDE_DIR} ${BROTLI_DECODE_INCLUDE_DIR})
find_library(BROTLI_COMMON_LIB brotlicommon REQUIRED)
find_library(BROTLI_ENC_LIB brotlienc REQUIRED)
find_library(BROTLI_DEC_LIB brotlidec REQUIRED)
set(BROTLI_LIB ${BROTLI_COMMON_LIB} ${BROTLI_ENC_LIB} ${BROTLI_DEC_LIB})

find_package(cppcoro REQUIRED)

get_property(ImportedTargets DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY IMPORTED_TARGETS)
message("Imported targets: ${ImportedTargets}")

add_library(libmirism INTERFACE)
target_include_directories(libmirism INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include ${BACKTRACE_INCLUDE_DIR}
	${TGBOTCPP_INCLUDE_DIR} ${BROTLI_INCLUDE_DIR})
target_link_libraries(libmirism INTERFACE fmt::fmt nameof::nameof Boost::headers Boost::iostreams ${BACKTRACE_LIB} ${TGBOTCPP_LIB}
	magic_enum::magic_enum OpenSSL::SSL OpenSSL::Crypto ${BROTLI_LIB} cppcoro::cppcoro)
target_compile_features(libmirism INTERFACE cxx_std_23)
target_compile_options(libmirism INTERFACE -Wno-pmf-conversions)

option(MIRISM_ENABLE_HTTPLIB "Enable httplib backend" OFF)
if(MIRISM_ENABLE_HTTPLIB)
	find_package(httplib REQUIRED)
	target_link_libraries(libmirism INTERFACE httplib::httplib)
endif()

add_subdirectory(test)
