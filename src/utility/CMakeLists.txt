add_library(mirism_utility SHARED common.cpp string.cpp logger.cpp compression.cpp)
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
