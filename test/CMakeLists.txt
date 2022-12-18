add_executable(test_utility utility.cpp)
target_link_libraries(test_utility mirism::utility)
add_test(utility test_utility)

set_property(TARGET test_utility PROPERTY CXX_STANDARD 23)
set_property(TARGET test_utility PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET test_utility PROPERTY CXX_EXTENSIONS OFF)
