if(NOT LIBMIRISM_BUILD_UTILITY)
	find_package(mirism REQUIRED COMPONENTS utility)
endif()

find_package(magic_enum REQUIRED)

add_library(mirism_framework SHARED handler/base.cpp)
add_library(mirism::framework ALIAS mirism_framework)
target_include_directories(mirism_framework PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(mirism_framework PUBLIC mirism::utility)
target_link_libraries(mirism_framework PRIVATE magic_enum::magic_enum)
set_property(TARGET mirism_framework PROPERTY CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
set_property(TARGET mirism_framework PROPERTY VERSION ${PROJECT_VERSION})
install(TARGETS mirism_framework EXPORT MirismConfig LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../include/mirism/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mirism
	FILES_MATCHING PATTERN "framework.hpp")
export(TARGETS mirism_framework FILE MirismConfig.cmake)
