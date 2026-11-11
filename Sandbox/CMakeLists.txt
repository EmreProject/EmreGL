add_executable(Sandbox    "03_basic_rendering/main.cpp")
target_link_libraries(Sandbox PRIVATE Renderer)
target_compile_definitions(Sandbox PRIVATE ENGINE_ASSETS_DIR="${ENGINE_ASSETS_DIR}")
target_compile_definitions(Sandbox PRIVATE ENGINE_MATERIAL_SHADERS_DIR="${ENGINE_MATERIAL_SHADERS_DIR}")
target_compile_definitions(Sandbox PRIVATE SANDBOX_PATH="${CMAKE_CURRENT_SOURCE_DIR}")

if(BUILD_TESTING)
    add_test(NAME sandbox-smoke COMMAND Sandbox)
endif()
