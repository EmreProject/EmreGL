# Header-only placeholder until the math implementation is added.
# ogl_Matrix.h requires missing ogl_Vector.h, ogl_Projection.h and Assimp.
add_library(EmreMath INTERFACE)
target_include_directories(EmreMath INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(EmreMath INTERFACE cxx_std_20)
