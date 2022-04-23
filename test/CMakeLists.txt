add_executable(test_common common.cpp)
target_link_libraries(test_common libmirism)
add_test(common test_common)

add_executable(test_atomic atomic.cpp)
target_link_libraries(test_atomic libmirism)
add_test(atomic test_atomic)

add_executable(test_pipe pipe.cpp)
target_link_libraries(test_pipe libmirism)
add_test(pipe test_pipe)
