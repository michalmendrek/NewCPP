find_package(Threads REQUIRED)

#add_executable(ml_controller_tests
#  ml_controller_test.cpp
#)

#target_link_libraries(ml_controller_tests
#  MLController
#  gmock_main)

#add_test(NAME ml_controller_tests
#    COMMAND ./ml_controller_tests
#)

#Tools Tests
add_executable(ml_tools_tests
  sliding_window_test.cpp
  sliding_window_mrg_test.cpp
  ticker_test.cpp
)

target_link_libraries(ml_tools_tests
  gmock_main)

add_test(NAME ml_tools_tests
    COMMAND ./ml_tools_tests
)

#Networking test
add_executable(ml_networking_tests
  ipc_test.cpp
  ipc_test.h
  generic_networking_test.cpp
)

target_link_libraries(ml_networking_tests
  gmock_main)

add_test(NAME ml_networking_tests
    COMMAND ./ml_networking_tests
)
