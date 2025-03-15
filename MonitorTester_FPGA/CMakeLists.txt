# Verilator finding needs to be more robust for Windows
find_package(verilator HINTS $ENV{VERILATOR_ROOT} "$ENV{PROGRAMFILES}/verilator")
if (NOT verilator_FOUND)
    message(FATAL_ERROR "Verilator not found. Please install Verilator or set VERILATOR_ROOT environment variable.")
endif()

set(INCLUDE_DIRS "src/include")

function (add_module MODULE_NAME)
    set(options)
    set(args PREFIX TOP_MODULE)
    set(lists SOURCES RESOURCE_DIRS)
    cmake_parse_arguments(ADD_MODULE "${options}" "${args}" "${lists}" "${ARGN}")

    add_library(${MODULE_NAME} SHARED)

    # Silent warnings on verilated code
    set_target_properties(${MODULE_NAME} PROPERTIES 
        INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:${MODULE_NAME},INTERFACE_INCLUDE_DIRECTORIES>)

    set(DEFINES "")
    if (ADD_MODULE_RESOURCE_DIRS)
        list(LENGTH ADD_MODULE_RESOURCE_DIRS LEN)
        set(IDX 0)
        while (IDX LESS LEN)
            list(GET ADD_MODULE_RESOURCE_DIRS ${IDX} RES)
            file(TO_CMAKE_PATH "${RES}" RES_PATH)
            file(TO_CMAKE_PATH "${RESOURCES_DIR}/${RES}" DST_PATH)
            message("Resource ${RES} -> ${DST_PATH}")
            math(EXPR IDX "${IDX} + 1")

            # Check if there's a define flag following
            if(IDX LESS LEN)
                list(GET ADD_MODULE_RESOURCE_DIRS ${IDX} DEF)
                if (${DEF} MATCHES "^-D.*")
                    message("Define ${DEF}")
                    # Ensure path format is correct for compiler
                    if(MSVC)
                        # Double backslashes for Windows paths in defines
                        string(REPLACE "/" "\\\\" WINDOWS_PATH "${DST_PATH}")
                        list(APPEND DEFINES "${DEF}=\"${WINDOWS_PATH}\"")
                    else()
                        list(APPEND DEFINES "${DEF}=\"${DST_PATH}\"")
                    endif()
                    math(EXPR IDX "${IDX} + 1")
                endif()
            endif()
        endwhile()
    endif()

    set(MODULE_VERILOG_SOURCES ${ADD_MODULE_SOURCES})

    # Add Windows-specific verilator args if necessary
    set(VERILATOR_PLATFORM_ARGS)
    if(MSVC)
        list(APPEND VERILATOR_PLATFORM_ARGS --compiler msvc)
    endif()

    verilate(${MODULE_NAME} 
        SOURCES ${MODULE_VERILOG_SOURCES} 
        INCLUDE_DIRS "." 
        PREFIX ${ADD_MODULE_PREFIX} 
        TOP_MODULE ${ADD_MODULE_TOP_MODULE} 
        VERILATOR_ARGS -Wall -Wno-fatal -cc ${DEFINES} ${VERILATOR_PLATFORM_ARGS})

    # Configure MSVC-specific settings
    if(MSVC)
        target_compile_options(${MODULE_NAME} PRIVATE /EHsc)
        target_compile_definitions(${MODULE_NAME} PRIVATE _USE_MATH_DEFINES NOMINMAX)
    endif()
endfunction()

add_module(MONITOR_TESTER 
    SOURCES Monitor_Tester.v
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
    PREFIX Vmonitor_tester 
    TOP_MODULE Monitor_Tester)
