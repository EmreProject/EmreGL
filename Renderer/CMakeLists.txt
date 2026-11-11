add_library(Renderer STATIC
    src/Shaders.cpp
    src/ogl_anatutorial_v2.cpp
    include/Shaders.h
    include/ogl_anatutorial_v2.h
    src/ogl_axis_debug.cpp
    src/ogl_Camera.cpp
    src/ogl_default_meshes.cpp
    src/ogl_lightning_v2.cpp
    src/ogl_model_v2.cpp
    
    
    src/ogl_shader_v2.cpp
    
    src/ogl_texture.cpp
    src/ogl_World.cpp
    src/oglAssimp.cpp
    src/oglComputeShader.cpp
    src/oglControls.cpp
    src/oglMaterialConstants.cpp
    src/oglMaterialShader.cpp
    src/oglMeshes.cpp
    src/oglMouseCoord.cpp
    src/oglMouseEvents.cpp
    src/oglPostProcessing.cpp
    src/oglRenderer.cpp
    src/oglRenderTarget.cpp
    src/oglScene.cpp
    src/oglShadowMap.cpp
    src/oglWindow.cpp

    include/ogl_axis_debug.h
    include/ogl_Camera.h
    include/ogl_default_meshes.h
    include/ogl_lightning_v2.h
    include/ogl_model_v2.h
    
    include/ogl_Projection.h
    include/ogl_shader_v2.h
    
    include/ogl_texture.h
    include/ogl_World.h
    include/oglAssimp.h
    include/oglComputeShader.h
    include/oglControls.h
    include/oglMaterialConstants.h
    include/oglMaterialShader.h
    include/oglMeshes.h
    include/oglMouseCoord.h
    include/oglMouseEvents.h
    include/oglPostProcessing.h
    include/oglRenderer.h
    include/oglRenderTarget.h
    include/oglScene.h
    include/oglShadowMap.h
    include/oglWindow.h
 "include/ogl_anatutorial_v2.h"  "include/Shaders.h" "src/Shaders.cpp" "src/ogl_anatutorial_v2.cpp")

target_include_directories(Renderer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(Renderer PUBLIC cxx_std_20)
target_compile_definitions(Renderer PRIVATE
    ENGINE_MATERIAL_SHADERS_DIR="${ENGINE_MATERIAL_SHADERS_DIR}"
)
target_link_libraries(Renderer PUBLIC EmreMath EmreGUI stb_image glfw glew_s assimp::assimp)
