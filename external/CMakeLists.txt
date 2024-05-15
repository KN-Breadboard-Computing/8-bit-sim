# RAYLIB
set(RAYLIB_VERSION 4.5)
find_package(raylib ${RAYLIB_VERSION} QUIET)

# this magic formula silences warnings from raylib
set_target_properties(raylib PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:raylib,INTERFACE_INCLUDE_DIRECTORIES>)

# DOCTEST
add_library(Doctest INTERFACE)
target_include_directories(Doctest INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/doctest)

# IMGUI
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(IMGUI_RAYLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rlimgui)
add_library(Imgui STATIC ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp ${IMGUI_RAYLIB_DIR}/rlImGui.cpp)
target_include_directories(Imgui PUBLIC ${IMGUI_DIR}/include ${IMGUI_RAYLIB_DIR})
set_target_properties(Imgui PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:Imgui,INTERFACE_INCLUDE_DIRECTORIES>)
