find_package(verilator HINTS $ENV{VERILATOR_ROOT})
if (NOT verilator_FOUND)
    message(FATAL_ERROR "Verilator not found")
endif()

add_executable(alu_test ALU_test.cpp)
target_link_libraries(alu_test Doctest ALU)
add_test(NAME alu_test COMMAND alu_test)
