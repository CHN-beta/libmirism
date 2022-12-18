add_library(mirism_utility SHARED misc/common.cpp misc/string.cpp misc/logger.cpp misc/compression.cpp)
add_library(mirism::utility ALIAS mirism_utility)
target_include_directories(mirism_utility PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include ${TGBOTCPP_INCLUDE_DIR}
	${BACKTRACE_INCLUDE_DIR} ${BROTLI_INCLUDE_DIR})
target_link_libraries(mirism_utility PUBLIC fmt::fmt magic_enum::magic_enum cppcoro::cppcoro nameof::nameof
	${TGBOTCPP_LIB} Boost::headers Boost::iostreams ${BACKTRACE_LIB} ${BROTLI_LIB})
set_property(TARGET mirism_utility PROPERTY CXX_STANDARD 23)
set_property(TARGET mirism_utility PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET mirism_utility PROPERTY CXX_EXTENSIONS OFF)
