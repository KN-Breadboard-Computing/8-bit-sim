find_package(verilator HINTS $ENV{VERILATOR_ROOT})
if (NOT verilator_FOUND)
    message(FATAL_ERROR "Verilator not found")
endif()

set(INCLUDE_DIRS "src/include")

function (add_module MODULE_NAME)
    set(options)
    set(args PREFIX TOP_MODULE)
    set(lists SOURCES)
    cmake_parse_arguments(ADD_MODULE "${options}" "${args}" "${lists}" "${ARGN}")

    add_library(${MODULE_NAME} SHARED)

    set(MODULE_VERILOG_SOURCES ${ADD_MODULE_SOURCES})
    verilate(${MODULE_NAME} SOURCES ${MODULE_VERILOG_SOURCES} INCLUDE_DIRS "." PREFIX ${ADD_MODULE_PREFIX} TOP_MODULE ${ADD_MODULE_TOP_MODULE} VERILATOR_ARGS -Wall -cc)
endfunction()

add_module(TRISTATE_BUFFER SOURCES basics/tristate_buffer.v)
add_module(COUNTER SOURCES basics/counter.v)
add_module(REGISTER SOURCES basics/register.v)
# add_module(CPU cpu/cpu.v basics/counter.v basics/tristate_buffer.v basics/register.v cpu/alu.v)
add_module(ALU SOURCES cpu/alu.v)
add_module(CONTROL_UNIT SOURCES cpu/control_unit.v)
add_module(RAM SOURCES adapters/ram_adapter.sv basics/ram.sv PREFIX Vram TOP_MODULE ram_adapter)
add_module(MEM_UNIT SOURCES adapters/mem_unit_adapter.sv cpu/mem_unit.sv PREFIX Vmem_unit TOP_MODULE mem_unit_adapter)
