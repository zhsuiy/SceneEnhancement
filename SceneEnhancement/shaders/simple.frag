#version 400 core
//in vec3 ourColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
out vec4 color;
uniform sampler2D ourTexture;
uniform sampler2D ourTexture1;
uniform float lambda;

uniform vec3 lightPos;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 viewPos;
void main()
{
	//color = mix(texture(ourTexture, TexCoord),texture(ourTexture1,TexCoord),lambda);
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir),0.0);
	vec3 diffuse = diff * lightColor;
	
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor;
	
	float specularStrenth = 0.5f;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrenth * spec * lightColor;
	
	color = vec4((ambient + diffuse + specular) * objectColor, 1.0f);
}