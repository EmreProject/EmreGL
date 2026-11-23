#version 430

layout (location = 0) out vec4 startPosition;
layout (location = 1) out vec4 startVelocity;
layout (location = 2) out vec4 targetPosition;

in float vId;
in float vTextureWidth;
in float vTextureHeight;
in vec2 vUv;
in vec2 vPlanePosition;
in mat4 vModelMatrix;

uniform float uParticleCount; // total number of particles
uniform float uRadius;          // e.g. 4.0
uniform float uStartVelocityMagnitude; // converted value from pixel to shader
uniform float uStartVelocityRotateRadian; // pass as radians

const float PI  = 3.14159265359;
const float PHI = 1.61803398875;             
const float GOLDEN_ANGLE = 2.0 * PI * (1.0 - 1.0 / PHI);   



vec3 fibonacciSphere(float id, float N, float R)
{
    float offset = 2.0 / float(N);
    float y      = -1.0 + offset * (id + 0.5);
    float r      = sqrt(max(0.0, 1.0 - y * y));

    float theta  = GOLDEN_ANGLE * id;

    float x = r * cos(theta);
    float z = r * sin(theta);

    return vec3(x, y, z) * R;   
}



vec3 axisFromTwoVectors(vec3 v1, vec3 v2)
{
    return normalize(cross(v1, v2));  
}



vec3 rotateAround(vec3 v, vec3 axis, float angle)
{
    float  s  = sin(angle);
    float  c  = cos(angle);
    float  ic = 1.0 - c;                 
    return v * c + cross(axis, v) * s + axis * dot(axis, v) * ic;
}

void main()
{           

    vec4 startPos = vModelMatrix * vec4(vPlanePosition.x, 0.0, vPlanePosition.y,1.);
    vec3 targetPos = fibonacciSphere(vId, uParticleCount, uRadius);
    vec3 startToTarget= targetPos - startPos.xyz;

    vec3 axisOfRotation= axisFromTwoVectors(startPos.xyz, startToTarget);
    vec3 vRotated= rotateAround(startToTarget, axisOfRotation, uStartVelocityRotateRadian);
    vRotated= normalize(vRotated) * uStartVelocityMagnitude;

    startPosition = vec4(startPos.xyz, 1.0);
    startVelocity = vec4(vRotated, 1.0);
    targetPosition = vec4(targetPos,1.0);
}