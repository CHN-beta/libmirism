add_executable(test_misc_common misc/common.cpp)
target_link_libraries(test_misc_common libmirism)
add_test(misc_common test_misc_common)

add_executable(test_misc_concepts misc/concepts.cpp)
target_link_libraries(test_misc_concepts libmirism)
add_test(misc_concepts test_misc_concepts)

add_executable(test_misc_atomic misc/atomic.cpp)
target_link_libraries(test_misc_atomic libmirism)
add_test(misc_atomic test_misc_atomic)

add_executable(test_misc_string misc/string.cpp)
target_link_libraries(test_misc_string libmirism)
add_test(misc_string test_misc_string)

add_executable(test_misc_format misc/format.cpp)
target_link_libraries(test_misc_format libmirism)
add_test(misc_format test_misc_format)

add_executable(test_misc_staticize misc/staticize.cpp)
target_link_libraries(test_misc_staticize libmirism)
add_test(misc_staticize test_misc_staticize)

add_executable(test_misc_logger misc/logger.cpp)
target_link_libraries(test_misc_logger libmirism)
add_test(misc_logger test_misc_logger)

add_executable(test_misc_pipe misc/pipe.cpp)
target_link_libraries(test_misc_pipe libmirism)
add_test(misc_pipe test_misc_pipe)

add_executable(test_client_base client/base.cpp)
target_link_libraries(test_client_base libmirism)
add_test(client_base test_client_base)

add_executable(test_client_httplib client/httplib.cpp)
target_link_libraries(test_client_httplib libmirism)
add_test(client_httplib test_client_httplib)

add_executable(test_instance instance.cpp)
target_link_libraries(test_instance libmirism)
add_test(instance test_instance)

add_executable(test_httplib httplib.cpp)
target_link_libraries(test_httplib libmirism)
add_test(httplib test_httplib)
