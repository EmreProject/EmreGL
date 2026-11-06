add_executable(Sandbox main.cpp)
target_link_libraries(Sandbox PRIVATE EmreGL::Engine)
target_compile_definitions(Sandbox PRIVATE ENGINE_ASSETS_DIR="${ENGINE_ASSETS_DIR}")
