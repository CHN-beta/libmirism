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

add_library(mirism_utility SHARED common.cpp compression.cpp http.cpp logger.cpp string.cpp)
add_library(mirism::utility ALIAS mirism_utility)
target_include_directories(mirism_utility PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_include_directories(mirism_utility PUBLIC ${TGBOTCPP_INCLUDE_DIR} ${BACKTRACE_INCLUDE_DIR}
	PRIVATE ${BROTLI_INCLUDE_DIR})
target_link_libraries(mirism_utility PUBLIC fmt::fmt magic_enum::magic_enum cppcoro::cppcoro nameof::nameof
	${TGBOTCPP_LIB} Boost::headers Boost::iostreams ${BACKTRACE_LIB} ${BROTLI_LIB})
set_property(TARGET mirism_utility PROPERTY CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
set_property(TARGET mirism_utility PROPERTY VERSION ${PROJECT_VERSION})
install(TARGETS mirism_utility EXPORT MirismConfig LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../include/mirism/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mirism
	FILES_MATCHING PATTERN "utility.hpp" PATTERN "detail_/utility/*")
export(TARGETS mirism_utility FILE MirismConfig.cmake)
