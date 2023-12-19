add_executable(test1 test1.cpp)

# set variable libs to be a list
set(libs ALU)

target_link_libraries(test1 ${libs})
