#version 430

in vec2 TexCoord0;
in vec3 Normal0;
in vec3 LocalPos0;
in vec4 FragPos;

vec3 Nws;
mat3 tbn;
<in_normal_map>


in mat3 vNormalMatrix; // CPU sends transpose(inverse(mat3(modelMatrix)))
in vec3 vWorldNormal;
<environment_map_fragment>

<out_fragment_color>


uniform vec3 gCameraLocalPos;

<assimp_transparency>

<common>
<color_fragment>
<texture_fragment>
<color_and_texture_fragment> //if both texture and colors are set

<shadow_map_fragment>

<normal_map>

<light_common>
<light_directional>
<light_attenuation>
<light_point>
<light_spot>


//debug
//in vec3 vTangent;


void main()
{
    

    vec3 empty = gCameraLocalPos;

    <begin_fragment>
    <begin_fragment_result>
    <fragment_color_result>
    <fragment_texture_result>
    <fragment_color_and_texture_result>
    <begin_fragment_manual_gamma> //if no srgb used convert ggamma manually
    

    <begin_normal_fragment>
    <begin_normal_map_fragment>
  

    <light_main_begin>
    <light_main_directional>
    <light_main_point>
    <light_main_spot>
    <light_main_end>

    <end_fragment_gamma_enabled>
    <end_fragment_result>
    <end_fragment_result_assimp>
 // FragColor=vec4(vTangent,1.);

    <end_fragment_result_environment_map>
   
}
