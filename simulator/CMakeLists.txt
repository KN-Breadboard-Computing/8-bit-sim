set(EXEC_NAME "simulator")
add_executable(${EXEC_NAME} main.cpp)

if(SANITIZER_AVAILABLE_AND_SET)
  target_compile_options(${EXEC_NAME} PRIVATE ${SANITIZER_FLAGS})
  target_link_libraries(${EXEC_NAME} ${SANITIZER_FLAGS})
endif()

target_link_libraries(${EXEC_NAME} GPU MONITOR_TESTER raylib Imgui)

if(MSVC)
  set_target_properties(${EXEC_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
  )
endif()
