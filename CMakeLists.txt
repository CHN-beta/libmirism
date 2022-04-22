cmake_minimum_required(VERSION 3.14)
project(libmirism)
enable_testing()

add_library(libmirism INTERFACE)
target_include_directories(libmirism INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_property(TARGET libmirism PROPERTY CXX_STANDARD 23)
set_property(TARGET libmirism PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET libmirism PROPERTY CXX_EXTENSIONS OFF)

add_subdirectory(test)
