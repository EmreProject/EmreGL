#version 430


<attributes_from_geometry>
<attributes_bone>

uniform mat4 viewProjectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
<common> //hep var
<color_vertex> //color hook varsa


out vec2 TexCoord0;
out vec3 Normal0;
out vec3 LocalPos0;
out vec4 FragPos;
<out_normal_map>

<displacement_map_vertex>

uniform mat3 normalMatrix; // CPU sends transpose(inverse(mat3(modelMatrix)))
out mat3 vNormalMatrix; 
out vec3 vWorldNormal;

void main()
{
    <begin_vertex> //hep var
    <begin_color_vertex> //color hook varsa
	
    <begin_normal_map_vertex>
    <displacement_map_vertex_begin>

    <end_color_vertex>//color hook varsa
    <skinning_vertex_main>
    gl_Position = viewProjectionMatrix * modelMatrix * vec4(transformedPosition,1.0);
    

    FragPos = modelMatrix * vec4(transformedPosition,1.0);

  
    transformedNormal=normalize(transformedNormal);

    TexCoord0 = transformedUv;
    Normal0 = transformedNormal;
    LocalPos0 = transformedPosition;

   vWorldNormal = normalize(normalMatrix * transformedNormal);
   vNormalMatrix = normalMatrix;


}
