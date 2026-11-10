#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;

uniform mat4 gWVP;

out vec2 TexCoord0;
out vec3 Normal0;
out vec3 LocalPos0;
out mat3 TBN;


void main()
{
    gl_Position = gWVP * vec4(Position, 1.0);
    TexCoord0 = TexCoord;
    Normal0 = Normal;
    LocalPos0 = Position;

  // TBN matrix remains the same, as we operate in local space
    vec3 T = normalize(Tangent);
    vec3 N = normalize(Normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    TBN = transpose(mat3(T, B, N)); //inverse of tbn that it
   //transforms local space to tangent space

}
