add_library(PS2 STATIC ps2/ps2.cpp)
target_include_directories(PS2 PUBLIC ps2)

set(EXEC_NAME "simulator")
add_executable(${EXEC_NAME} main.cpp)

if(SIMULATOR_COMPILE_OPTIONS)
  target_compile_options(${EXEC_NAME} PRIVATE ${SIMULATOR_COMPILE_OPTIONS})
endif()

if(SIMULATOR_COMPILE_DEFINITIONS)
  target_compile_definitions(${EXEC_NAME} PRIVATE ${SIMULATOR_COMPILE_DEFINITIONS})
endif()

if(SANITIZER_AVAILABLE_AND_SET)
  target_compile_options(${EXEC_NAME} PRIVATE ${SANITIZER_FLAGS})
  target_link_libraries(${EXEC_NAME} ${SANITIZER_FLAGS})
endif()

target_link_libraries(${EXEC_NAME} GPU CPU PS2 MONITOR_TESTER raylib Imgui fmt Expected)

if(MSVC)
  set_target_properties(${EXEC_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
  )
endif()
