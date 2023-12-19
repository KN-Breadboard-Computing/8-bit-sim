cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
    VerilogProject
    VERSION 0.1
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    include(CTest)

    enable_testing()
endif()

include(CheckCXXCompilerFlag)
option(ENABLE_CUSTOM_COMPILER_FLAGS "Enables custom compiler flags" ON)
if (ENABLE_CUSTOM_COMPILER_FLAGS)
    list(APPEND custom_compiler_flags
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Wconversion
    )
endif()

add_subdirectory(external)
add_subdirectory(src)

option(ENABLE_TESTBENCHES "Enables testbenches" ON)
if (ENABLE_TESTBENCHES)
    add_subdirectory(testbenches)
endif()

option(ENABLE_TESTS "Enables tests" ON)
if (ENABLE_TESTS)
    add_subdirectory(tests)
endif()

option(ENABLE_EXAMPLES "Enables examples" ON)
if (ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif()
