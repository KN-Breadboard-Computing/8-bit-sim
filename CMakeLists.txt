cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
    VerilogProject
    VERSION 0.1
)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src)
add_subdirectory(testbenches)
