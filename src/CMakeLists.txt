find_package(verilator HINTS $ENV{VERILATOR_ROOT})
if (NOT verilator_FOUND)
    message(FATAL_ERROR "Verilator not found")
endif()

function (add_module MODULE_NAME)
    add_library(${MODULE_NAME} SHARED)

    set(MODULE_VERILOG_SOURCES ${ARGN})
    verilate(${MODULE_NAME} SOURCES ${MODULE_VERILOG_SOURCES} INCLUDE_DIRS "." VERILATOR_ARGS -Wall -cc)
endfunction()

add_module(TRISTATE_BUFFER basics/tristate_buffer.v)
add_module(REGISTER basics/register.v)
add_module(CPU cpu/cpu.v)
add_module(ALU cpu/alu.v)
add_module(CONTROL_UNIT cpu/control_unit.v)
