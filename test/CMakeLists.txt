add_executable(test_common common.cpp)
target_link_libraries(test_common libmirism)
add_test(common test_common)

add_executable(test_atomic atomic.cpp)
target_link_libraries(test_atomic libmirism)
add_test(atomic test_atomic)

add_executable(test_pipe pipe.cpp)
target_link_libraries(test_pipe libmirism)
add_test(pipe test_pipe)

add_executable(test_logger logger.cpp)
target_link_libraries(test_logger libmirism)
add_test(logger test_logger)

add_executable(test_instance instance.cpp)
target_link_libraries(test_instance libmirism)
add_test(instance test_instance)
