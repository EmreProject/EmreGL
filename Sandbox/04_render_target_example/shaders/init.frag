#version 430

layout (location = 0) out vec4 newPos;

in vec2 vUv;

uniform float maxRadius;
uniform float minRadius;
uniform float maxHeight; 


const float PI = 3.14159265358979323846;
const float TWO_PI   = 6.28318530717958647692;

float hash(vec2 p)
{
    // Two typical hashes...
	return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
    
    // This one is better, but it still stretches out quite quickly...
    // But it's really quite bad on my Mac(!)
    //return fract(sin(dot(p, vec2(1.0,113.0)))*43758.5453123);

}


void main()
{           
   
    
    float value = hash(vUv.xy);
    float value2=hash(vUv.yx);
    float value3=hash(vUv.yy);

    float radius = mix(minRadius, maxRadius, value);
    float radian= value2 * TWO_PI;
    float height= mix(-maxHeight/2., maxHeight/2., value3);   
   
   vec3 position= vec3(radius * cos(radian), 0. , radius * sin(radian));
    position.y = height;

    newPos = vec4(position,1.);
}