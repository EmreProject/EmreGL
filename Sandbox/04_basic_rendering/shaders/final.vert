#version 430


layout (location = 1) in vec3 normal;
layout (location = 0) in vec2 uv;
layout (location = 2) in vec3 position;


uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;

uniform sampler2D positionData;
out vec2 vUv;

void main()
{
    
   // vec3 movePos= vec3(texture(positionData, uv).x) * normal;
   vUv= uv;
   vec3 pos= vec3(texture(positionData, uv).xyz);
   gl_Position =  projectionMatrix * viewMatrix * modelMatrix * vec4(pos,1.0);
    
     gl_PointSize = 1.0;

}
