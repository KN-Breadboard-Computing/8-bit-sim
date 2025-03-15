find_package(verilator HINTS $ENV{VERILATOR_ROOT} "$ENV{PROGRAMFILES}/verilator")
if (NOT verilator_FOUND)
    message(FATAL_ERROR "Verilator not found. Please install Verilator or set VERILATOR_ROOT environment variable.")
endif()

function(add_verilator_test TEST_NAME)
    add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
    target_link_libraries(${TEST_NAME} Doctest ${ARGN})

    # Add Windows-specific settings
    if(MSVC)
        target_compile_options(${TEST_NAME} PRIVATE /EHsc)
        set_target_properties(${TEST_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
        )
    endif()

    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

add_verilator_test(gpu_test GPU)
