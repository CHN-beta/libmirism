if(NOT LIBMIRISM_BUILD_FRAMEWORK)
	find_package(mirism REQUIRED COMPONENTS framework)
endif()

find_package(httplib REQUIRED)

add_library(mirism_httplib SHARED client.cpp server.cpp)
add_library(mirism::httplib ALIAS mirism_httplib)
target_include_directories(mirism_httplib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(mirism_httplib PRIVATE httplib::httplib)
set_property(TARGET mirism_httplib PROPERTY CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
set_property(TARGET mirism_httplib PROPERTY VERSION ${PROJECT_VERSION})
install(TARGETS mirism_httplib EXPORT MirismConfig LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../include/mirism/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mirism
	FILES_MATCHING PATTERN "httplib.hpp")
export(TARGETS mirism_httplib FILE MirismConfig.cmake)
