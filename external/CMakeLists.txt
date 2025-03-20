# Include FetchContent and required dependencies
include(FetchContent)

# EXPECTED
add_library (Expected INTERFACE)
target_include_directories(Expected INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/expected)

# FMT
find_package(fmt QUIET)
if (NOT fmt_FOUND)
    FetchContent_Declare(
      fmt
      GIT_REPOSITORY https://github.com/fmtlib/fmt
      GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1
    FetchContent_MakeAvailable(fmt)
endif()

# RAYLIB
set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET)
if (NOT raylib_FOUND)
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        # Disable OpenGL ES version - use standard OpenGL for better Windows compatibility
        set(PLATFORM "Desktop" CACHE STRING "" FORCE)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE) # Static linking is more reliable on Windows
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()

# This magic formula silences warnings from raylib
set_target_properties(raylib PROPERTIES 
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:raylib,INTERFACE_INCLUDE_DIRECTORIES>)

# DOCTEST
add_library(Doctest INTERFACE)
target_include_directories(Doctest INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/doctest)

# IMGUI
file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/imgui" IMGUI_DIR)
file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/rlimgui" IMGUI_RAYLIB_DIR)

add_library(Imgui STATIC 
    ${IMGUI_DIR}/imgui.cpp 
    ${IMGUI_DIR}/imgui_demo.cpp 
    ${IMGUI_DIR}/imgui_draw.cpp 
    ${IMGUI_DIR}/imgui_tables.cpp 
    ${IMGUI_DIR}/imgui_widgets.cpp 
    ${IMGUI_RAYLIB_DIR}/rlImGui.cpp)

target_link_libraries(Imgui raylib)
target_include_directories(Imgui PUBLIC ${IMGUI_DIR} ${IMGUI_RAYLIB_DIR})

# Windows-specific flags for ImGui
if(MSVC)
    target_compile_definitions(Imgui PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

set_target_properties(Imgui PROPERTIES 
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:Imgui,INTERFACE_INCLUDE_DIRECTORIES>)

# fmt
find_package(fmt QUIET)
if (NOT fmt_FOUND)
    FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG master
        UPDATE_DISCONNECTED TRUE # Better for Windows environments
    )
    FetchContent_MakeAvailable(fmt)
endif()
