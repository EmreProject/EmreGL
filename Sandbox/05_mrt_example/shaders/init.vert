#version 430


layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in float id;



uniform mat4 viewProjectionMatrix;
uniform mat4 modelMatrix;


uniform float uTextureWidth;
uniform float uTextureHeight;

out vec2 vUv;
out float vId;
out float vTextureWidth;
out float vTextureHeight;
out vec2 vPlanePosition;
out mat4 vModelMatrix;

void main()
{
    
     
    vec2 pixelPosition = vec2(0.,0.);
    pixelPosition.x=mod(id,uTextureWidth);
    pixelPosition.y=floor(id/uTextureWidth);


    vec2 realPosition=vec2(0.,0.);
    realPosition.x = -1. + ((pixelPosition.x + 0.5) / uTextureWidth)*2.;
    realPosition.y = -1. + ((pixelPosition.y+ 0.5) / uTextureHeight)*2.;


    vPlanePosition= realPosition;
    vUv= uv;
    vId=id;
    vTextureWidth = uTextureWidth;
    vTextureHeight = uTextureHeight;
    vModelMatrix = modelMatrix;


    mat4 bos= viewProjectionMatrix * modelMatrix;

    gl_Position  =  vec4(realPosition.xy,0.,1.);
    gl_PointSize= 1.0;

}
