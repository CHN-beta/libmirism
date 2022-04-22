add_executable(test_atomic atomic.cpp)
target_link_libraries(test_atomic libmirism)
add_test(atomic test_atomic)
