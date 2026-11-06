cmake_minimum_required(VERSION 3.21)
project(EmreGL VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
# Use X11 by default on Linux; Wayland can be enabled explicitly.
option(GLFW_BUILD_WAYLAND "Build GLFW Wayland support" OFF)
set(BUILD_UTILS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(External/glfw EXCLUDE_FROM_ALL)
add_subdirectory(External/glew/build/cmake EXCLUDE_FROM_ALL)
# Bundled GLEW only exports its installed include path upstream.
target_include_directories(glew_s PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/External/glew/include>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
foreach(config Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${config}" config_upper)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/bin")
endforeach()

include(CTest)
add_subdirectory(Assets)
add_subdirectory(Math)
add_subdirectory(GUI)
add_subdirectory(Renderer)
add_subdirectory(Sandbox)
