set(EXEC_NAME "simulator")

find_package(raylib 4.5 REQUIRED)

add_executable(${EXEC_NAME} main.cpp
        ps2.cpp
        ps2.hpp
        ps2Lookup.hpp)

set(MAIN_FLAGS ${COMPILE_FLAGS})

if(SANITIZER_AVAILABLE_AND_SET)
  set(MAIN_FLAGS ${MAIN_FLAGS} ${SANITIZER_FLAGS})
  target_link_libraries(${EXEC_NAME} ${SANITIZER_FLAGS})
endif()

target_compile_options(${EXEC_NAME} PRIVATE ${MAIN_FLAGS})

target_link_libraries(${EXEC_NAME} GPU raylib)
