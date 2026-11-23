#version 430


layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;



uniform mat4 viewProjectionMatrix;
uniform mat4 modelMatrix;

out vec2 vUv;

void main()
{
    
    vUv= uv;
    gl_Position = viewProjectionMatrix * modelMatrix * vec4(position,1.0);
    

}
