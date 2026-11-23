#version 430



layout (location = 0) out vec4 currentPosition;
layout (location = 1) out vec4 currentVelocity;


in vec2 vUv;



uniform sampler2D uCurrentPosition;
uniform sampler2D uCurrentVelocity;
uniform sampler2D uTargetPosition;

uniform float uMaxForce;
uniform float uMaxSpeed;
uniform float uRange;

const float PI  = 3.14159265359;
const float PHI = 1.61803398875;            



void main()
{           

    vec3 currentPos=texture(uCurrentPosition, vUv).xyz;
    vec3 currentVel=texture(uCurrentVelocity, vUv).xyz;
    vec3 targetPos=texture(uTargetPosition, vUv).xyz;

    vec3 desired=targetPos-currentPos;
    float dist=length(desired);

    if (dist < uRange) {

    desired = normalize(desired) * uMaxSpeed *(dist/uRange);

    }else{

    desired = normalize(desired) * uMaxSpeed;
    
    }

    vec3 steer=desired - currentVel;
    if(length(steer) > uMaxForce) {
        steer = normalize(steer) * uMaxForce;
    }

    currentVel += steer;
    if(length(currentVel) > uMaxSpeed) {
        currentVel = normalize(currentVel) * uMaxSpeed;
    }
    currentPos += currentVel;
    

    currentPosition = vec4(currentPos, 1.0);
       // currentPosition = vec4(1.,1.,1., 1.0);
    currentVelocity = vec4(currentVel, 1.0);


}