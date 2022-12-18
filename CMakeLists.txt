cmake_minimum_required(VERSION 3.14)
project(libmirism)
enable_testing()
include(GNUInstallDirs)

find_package(fmt REQUIRED)

find_package(magic_enum REQUIRED)

get_property(ImportedTargets DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY IMPORTED_TARGETS)
message("Imported targets: ${ImportedTargets}")
message("List of compile features: ${CMAKE_CXX_COMPILE_FEATURES}")

add_library(mirism_mirism SHARED src/detail_/misc/common.cpp)
add_library(mirism::mirism ALIAS mirism_mirism)
target_include_directories(mirism_mirism PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(mirism_mirism PRIVATE fmt::fmt magic_enum::magic_enum)
set_property(TARGET mirism_mirism PROPERTY CXX_STANDARD 23)
set_property(TARGET mirism_mirism PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET mirism_mirism PROPERTY CXX_EXTENSIONS OFF)
