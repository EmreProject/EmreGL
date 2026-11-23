#version 430

out vec4 FragColor;

uniform vec4 uColor;
uniform sampler2D uTextureParticle;

void main()
{          


vec4 color=uColor;
  vec2 uv = gl_PointCoord;
  vec4 tex = texture(uTextureParticle, uv);

    FragColor = vec4(tex);
     //FragColor = vec4(1.,1.,1.,1.);
}