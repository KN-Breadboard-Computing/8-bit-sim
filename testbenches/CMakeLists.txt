find_package(verilator HINTS $ENV{VERILATOR_ROOT})
if (NOT verilator_FOUND)
    message(FATAL_ERROR "Verilator not found")
endif()

add_executable(ALU_tb ALU_tb.cpp)
target_link_libraries(ALU_tb ALU)
# add_cpp_testbench(Stack testbenches/Stack_tb.cpp)
# add_cpp_testbench(Memory testbenches/Memory_tb.cpp)
