#version 400 core
//in vec3 ourColor;
in vec2 TexCoord;
out vec4 color;
uniform sampler2D ourTexture;
uniform sampler2D ourTexture1;
uniform float lambda;

void main()
{
color = mix(texture(ourTexture, TexCoord),texture(ourTexture1,TexCoord),lambda);
}