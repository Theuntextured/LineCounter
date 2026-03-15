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

FetchContent_Declare(
        implot
        GIT_REPOSITORY https://github.com/epezent/implot.git
        GIT_TAG master
)
FetchContent_MakeAvailable(implot)

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
        "${implot_SOURCE_DIR}/implot.cpp"
        "${implot_SOURCE_DIR}/implot_items.cpp"
        "${implot_SOURCE_DIR}/implot_demo.cpp"
)

list(APPEND SOURCES
        "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
)

# --- VERSIONING ---
set(BUILD_VERSION "v0.0.0-dev")

find_package(Git)
if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --tags --always --dirty
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
    )
    if(GIT_VERSION)
        set(BUILD_VERSION ${GIT_VERSION})
    endif()
endif()

if(DEFINED CLI_VERSION)
    set(BUILD_VERSION ${CLI_VERSION})
endif()

add_compile_definitions(APP_VERSION="${BUILD_VERSION}")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
        src
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${implot_SOURCE_DIR}
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

set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE ON)

if(MSVC)
    target_link_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>")
endif()