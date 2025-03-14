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
    verilate(${MODULE_NAME} SOURCES ${MODULE_VERILOG_SOURCES} INCLUDE_DIRS "." PREFIX ${ADD_MODULE_PREFIX} TOP_MODULE ${ADD_MODULE_TOP_MODULE} VERILATOR_ARGS -Wall -Wno-fatal -cc ${DEFINES})
endfunction()

add_module(MONITOR_TESTER SOURCES Monitor_Tester.v
    bouncingSquare.v
    debounce.v
    fillColorPattern.v
    gradient.v
    gridPattern.v
    hSync.v
    tvPattern.v
    VGA.v
    vSync.v
    my_dff.v
    clock_div.v
    PREFIX Vmonitor_tester TOP_MODULE Monitor_Tester)
