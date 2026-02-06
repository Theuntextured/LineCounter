cmake_minimum_required(VERSION 3.24)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(LineCounter VERSION 1.0.0 LANGUAGES CXX)

set(BUILD_SHARED_LIBS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)

include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG master
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG docking
)
FetchContent_MakeAvailable(imgui)

file(GLOB_RECURSE SOURCES
        "src/*.cpp"
        "src/*.h"
)

list(APPEND SOURCES
        "${imgui_SOURCE_DIR}/imgui.cpp"
        "${imgui_SOURCE_DIR}/imgui_demo.cpp"
        "${imgui_SOURCE_DIR}/imgui_draw.cpp"
        "${imgui_SOURCE_DIR}/imgui_tables.cpp"
        "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
)

list(APPEND SOURCES
        "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
        src
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw opengl32)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

if(MSVC)
    # FOR DEBUG:  Generate full debug info (/Z7), Link with debug info (/DEBUG), Keep unreferenced data (/OPT:NOREF)
    # FOR RELEASE: Optimization (/O2) is default. We just ensure no debug garbage is added.

    target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:/Z7>
    )

    target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:/DEBUG>
            $<$<CONFIG:Debug>:/OPT:NOREF>
            $<$<CONFIG:Release>:/OPT:REF>    # Remove unused functions in Release
            $<$<CONFIG:Release>:/OPT:ICF>    # Fold identical functions in Release
    )
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE OFF)
