# Assimp is provided by External/assimp in the root CMake project.

add_library(EmreMath STATIC
    src/ogl_Vector.cpp
    src/ogl_Matrix.cpp
    include/ogl_Vector.h
    include/ogl_Matrix.h
)

target_include_directories(EmreMath PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(EmreMath PUBLIC cxx_std_20)
target_link_libraries(EmreMath
    PUBLIC assimp::assimp
    PRIVATE glfw glew_s
)
