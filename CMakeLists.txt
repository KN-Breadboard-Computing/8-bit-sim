cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(8BitComputer VERSION 0.1)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

if(MSVC)
  set(SIMULATOR_COMPILE_OPTIONS /W4 /permissive- /MP /wd4100)
  set(SIMULATOR_COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS)
else()
  set(SIMULATOR_COMPILE_OPTIONS -Wall -Wextra -Wpedantic -Wconversion)
endif()

option(ENABLE_SANITIZERS "Enable sanitizers." OFF)
if(ENABLE_SANITIZERS)
  message("- SANITIZERS ENABLED")
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    set(SANITIZER_AVAILABLE_AND_SET
        TRUE
        CACHE BOOL "" FORCE)
    mark_as_advanced(SANITIZER_AVAILABLE_AND_SET)
    set(SANITIZER_FLAGS -fsanitize=address,undefined -fno-omit-frame-pointer)
  endif()
  if(NOT SANITIZER_AVAILABLE_AND_SET)
    message(VERBOSE "The option ENABLE_SANITIZERS is set but sanitizers not found.")
  endif()
endif()

file(TO_CMAKE_PATH "${CMAKE_SOURCE_DIR}/resources" RESOURCES_DIR)

option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
if(ENABLE_CLANG_TIDY)
  message("- CLANG-TIDY ENABLED")
  find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
  if(CLANG_TIDY_EXE)
    set(CMAKE_CXX_CLANG_TIDY
        "${CLANG_TIDY_EXE};--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy;--use-color"
    )
  else()
    message(
      WARNING
        "The option ENABLE_CLANG_TIDY is set but clang-tidy executable is not found."
    )
  endif()
endif()

option(ENABLE_CPPCHECK "Enable cppcheck" OFF)
if(ENABLE_CPPCHECK)
  message("- CPPCHECK ENABLED")
  find_program(CPPCHECK_EXE NAMES "cppcheck")
  if(CPPCHECK_EXE)
    set(CMAKE_CXX_CPPCHECK "${CPPCHECK_EXE};--enable=all;--inconclusive")
  else()
    message(
      WARNING
        "The option ENABLE_CPPCHECK is set but cppcheck executable is not found."
    )
  endif()
endif()

add_subdirectory(external)
add_subdirectory(MonitorTester_FPGA)
add_subdirectory(gpu)
add_subdirectory(cpu)
add_subdirectory(simulator)

option(ENABLE_TESTS "Enables tests" ON)
if(ENABLE_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()
