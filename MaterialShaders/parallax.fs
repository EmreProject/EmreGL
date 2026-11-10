#version 330

const int MAX_POINT_LIGHTS = 2;
const int MAX_SPOT_LIGHTS = 2;


struct MaterialSpecular{
    float SpecularIntensity; //specific to material like wood or gold
    float SpecularPower; //specific to material like wood or gold
};

struct BaseLight
{
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
};

struct DirectionalLight
{
    BaseLight Base;
    vec3 Direction;
};

struct Attenuation
{
    float Constant;
    float Linear;
    float Exp;
};

struct PointLight
{
    BaseLight Base;
    vec3 LocalPos;
    Attenuation Atten;
};

struct SpotLight
{
    PointLight Base;
    vec3 Direction;
    float Cutoff;
    
};


uniform DirectionalLight gDirectionalLight;
uniform int gNumPointLights;
uniform PointLight gPointLights[MAX_POINT_LIGHTS];
uniform int gNumSpotLights;
uniform SpotLight gSpotLights[MAX_SPOT_LIGHTS];

uniform MaterialSpecular materialSpecular;

uniform sampler2D gSampler;
uniform sampler2D gNormalMap;
uniform sampler2D gDepthMap;

uniform vec3 gCameraLocalPos;
uniform float height_scale;

in vec2 TexCoord0;
in vec3 Normal0;
in vec3 LocalPos0;
in mat3 TBN;

out vec4 FragColor;


vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * height_scale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(gDepthMap, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(gDepthMap, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(gDepthMap, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords; 
} 

vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal)
{
    vec4 AmbientColor = vec4(Light.Color, 1.0f) *
                        Light.AmbientIntensity ;

    float DiffuseFactor = max(dot(Normal, -LightDirection),0.);

    vec4 DiffuseColor = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);

    if (DiffuseFactor > 0) {
        DiffuseColor = vec4(Light.Color, 1.0f) *
                       Light.DiffuseIntensity *
                       DiffuseFactor;
        vec3 PixelToCamera = normalize(TBN * (gCameraLocalPos - LocalPos0));
        
        //***BLINN-PHONG LIGHT

     vec3 halfwayDir=normalize(PixelToCamera - LightDirection) ;  
     float spec = pow(max(dot(Normal, halfwayDir),0.), materialSpecular.SpecularPower);
    SpecularColor = vec4(gDirectionalLight.Base.Color * materialSpecular.SpecularIntensity * spec, 1.0f);
       
    }

    return (AmbientColor + DiffuseColor + SpecularColor);
}


vec4 CalcDirectionalLight(vec3 Normal)
{

    // Transform the light direction to tangent space
    vec3 LightDirection = normalize(TBN * gDirectionalLight.Direction);
    return CalcLightInternal(gDirectionalLight.Base, LightDirection, Normal);
}

vec4 CalcPointLight(PointLight l, vec3 Normal)
{

    // Transform the light direction to tangent space
    vec3 LightDirection = TBN*(LocalPos0 - l.LocalPos);
    float Distance = length(LightDirection);
    LightDirection = normalize(LightDirection); 

    vec4 Color = CalcLightInternal(l.Base, LightDirection, Normal);
    float Attenuation =  l.Atten.Constant +
                         l.Atten.Linear * Distance +
                         l.Atten.Exp * Distance * Distance;

    return Color / Attenuation;
}

vec4 CalcSpotLight(SpotLight l, vec3 Normal)
{
    vec3 LightToPixel = normalize(LocalPos0 - l.Base.LocalPos);
    LightToPixel = normalize(TBN * LightToPixel); // Transform to tangent space
    vec3 SpotDirection = normalize(TBN * l.Direction); // Transform spotlight direction to tangent space
    float SpotFactor = dot(LightToPixel, SpotDirection);
   // debug=vec3(abs(LocalPos0).x);
   // debug=(LightToPixel+vec3(1.))/vec3(2.);
    if (SpotFactor > l.Cutoff) {
        vec4 Color = CalcPointLight(l.Base, Normal);
        float SpotLightIntensity = (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - l.Cutoff));
           // debug=vec3(Color * SpotLightIntensity);
        return Color * SpotLightIntensity;
    }
    else {
        return vec4(0,0,0,0);
    }
}

void main()
{
    //DEPTH MAP

   //
   // vec3 gCameraTangentPos=TBN * gCameraLocalPos;
    //vec3 TangentPos0= TBN * LocalPos0;
    //vec3 viewDir=normalize(gCameraTangentPos-TangentPos0);
     vec3 viewDir = normalize(TBN * (gCameraLocalPos - LocalPos0));
    vec2 texCoords = TexCoord0;
    texCoords = ParallaxMapping(TexCoord0, viewDir);

    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0){
    discard;
    }


    //NORMAL MAP
     vec3 Normal = texture(gNormalMap, texCoords).rgb;
    // transform normal vector to range [-1,1]
    Normal = normalize(Normal * 2.0 - 1.0);  // this normal is in tangent space
    


    vec4 TotalLight = CalcDirectionalLight(Normal);

    for (int i = 0 ;i < gNumPointLights ;i++) {
        TotalLight += CalcPointLight(gPointLights[i], Normal);
    }

    for (int i = 0 ;i < gNumSpotLights ;i++) {
        TotalLight += CalcSpotLight(gSpotLights[i], Normal);
    }


    float gamma = 2.2;
    vec3 diffuseColor=pow(texture(gSampler, texCoords).rgb,vec3(gamma));
    diffuseColor *= TotalLight.rgb;


    FragColor.rgb = pow(diffuseColor.rgb, vec3( 1. / gamma));
   // FragColor= vec4(Normal,1.);
}
