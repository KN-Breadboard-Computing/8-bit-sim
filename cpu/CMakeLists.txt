find_package(verilator HINTS $ENV{VERILATOR_ROOT})
if (NOT verilator_FOUND)
    message(FATAL_ERROR "Verilator not found")
endif()

set(INCLUDE_DIRS "src/include")

# RESOURCE_DIRS key is a list of resource directories
#  - they will be copied/linked so that verilator can find them
#  - optionally a directory can be followed by -DVERILOG_DEFINE, 
#    which will cause a VERILOG_DEFINE to be defined with absolute path to that directory
function (add_module MODULE_NAME)
    set(options)
    set(args PREFIX TOP_MODULE)
    set(lists SOURCES RESOURCE_DIRS)
    cmake_parse_arguments(ADD_MODULE "${options}" "${args}" "${lists}" "${ARGN}")

    add_library(${MODULE_NAME} SHARED)

    # this magic formula silences warnings on the verilated libraries
    set_target_properties(${MODULE_NAME} PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:${MODULE_NAME},INTERFACE_INCLUDE_DIRECTORIES>)

    set(DEFINES "")

    if (ADD_MODULE_RESOURCE_DIRS)
        list(LENGTH ADD_MODULE_RESOURCE_DIRS LEN)
        set(IDX 0)
        while (IDX LESS LEN)
            list(GET ADD_MODULE_RESOURCE_DIRS ${IDX} RES)
            cmake_path(ABSOLUTE_PATH RES OUTPUT_VARIABLE RES_PATH)
            set(DST_PATH "${RESOURCES_DIR}/${RES}")
            message("Resource ${RES} -> ${DST_PATH}")

            math(EXPR IDX "${IDX} + 1")

            list(GET ADD_MODULE_RESOURCE_DIRS ${IDX} DEF)
            if (${DEF} MATCHES "^-D.*")
                message("Define ${DEF}")
                list(APPEND DEFINES "${DEF}=${DST_PATH}")
                math(EXPR IDX "${IDX} + 1")
            endif()
        endwhile()
    endif()

    set(MODULE_VERILOG_SOURCES ${ADD_MODULE_SOURCES})
    verilate(${MODULE_NAME} SOURCES ${MODULE_VERILOG_SOURCES} INCLUDE_DIRS "." PREFIX ${ADD_MODULE_PREFIX} TOP_MODULE ${ADD_MODULE_TOP_MODULE} VERILATOR_ARGS -Wall -cc ${DEFINES})
endfunction()

add_module(TRISTATE_BUFFER SOURCES basics/tristate_buffer.v)
add_module(COUNTER SOURCES basics/counter.v)
add_module(REGISTER SOURCES basics/register.v)
add_module(SHIFT_REG SOURCES basics/shift_reg.sv)
add_module(CPU SOURCES adapters/cpu_adapter.sv cpu/cpu.v basics/tristate_buffer.v basics/register.v cpu/alu.sv cpu/control_unit.v cpu/tmp.sv RESOURCE_DIRS roms -DROMS_PATH PREFIX Vcpu TOP_MODULE cpu_adapter)
add_module(ALU SOURCES cpu/alu.sv)
add_module(CONTROL_UNIT SOURCES cpu/control_unit.v RESOURCE_DIRS roms -DROMS_PATH)
add_module(RAM SOURCES adapters/ram_adapter.sv basics/ram.sv PREFIX Vram TOP_MODULE ram_adapter)
add_module(MEM_UNIT SOURCES adapters/mem_unit_adapter.sv cpu/mem_unit.sv PREFIX Vmem_unit TOP_MODULE mem_unit_adapter)
add_module(MODCOUNTER SOURCES gpu/modcounter.sv PREFIX Vmodcounter TOP_MODULE modcounter)
add_module(MODCOUNTER_TEST_WRAPPER SOURCES gpu/modcounter.sv gpu/modcounter_test_wrapper.sv PREFIX Vmodcounter_test_wrapper TOP_MODULE modcounter_test_wrapper)
# add_module(GPU SOURCES basics/shift_reg.sv gpu/gpu.sv RESOURCE_DIRS font -DFONT_PATH PREFIX Vgpu TOP_MODULE gpu)
add_module(TMP SOURCES cpu/tmp.sv adapters/tmp_adapter.sv PREFIX Vtmp TOP_MODULE tmp_adapter)
