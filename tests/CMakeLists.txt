find_package(verilator HINTS $ENV{VERILATOR_ROOT})
if (NOT verilator_FOUND)
    message(FATAL_ERROR "Verilator not found")
endif()

add_executable(tristate_buffer_test tristate_buffer_test.cpp)
target_link_libraries(tristate_buffer_test Doctest TRISTATE_BUFFER)
add_test(NAME tristate_buffer_test COMMAND tristate_buffer_test)
