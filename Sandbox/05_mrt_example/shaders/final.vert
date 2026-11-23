#version 430

layout (location = 3) in vec3 normal;
layout (location = 1) in vec3 position;
layout (location = 2) in vec2 uv;
layout (location = 0) in float id;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;

uniform sampler2D uPositionData;
uniform float uTextureWidth;
uniform float uTextureHeight;




void main()
{
 
    vec2 realUv=vec2(0.,0.);
    realUv.x =  (mod(id , uTextureWidth) + 0.5) / uTextureWidth ;
    realUv.y =  (floor(id / uTextureWidth) + 0.5) / uTextureHeight ;
   
   vec3 pos= vec3(texture(uPositionData, realUv).xyz);
  //pos.xy=realUv * 2.0 - 1.0;
 
   gl_Position =  projectionMatrix * viewMatrix * modelMatrix * vec4(pos,1.0);
    
    gl_PointSize = 4.0;

}
