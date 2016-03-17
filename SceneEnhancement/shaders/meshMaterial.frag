#version 400 core

struct DirLight {	
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
struct PointLight {
	vec3 position;
	// fading effect
	float constant;
	float linear;
	float quadratic;
	// attributes
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
struct Material {	
	bool useAmbientMap;
	bool useDiffuseMap;
	bool useSpecularMap;
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	sampler2D ambient;
	sampler2D diffuse;
	sampler2D specular;	
	float shininess;
	float transparent;
};

#define NR_POINT_LIGHTS 4
#define NR_MATERIAL 10

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 GetAmbientColor();
vec3 GetDiffuseColor();
vec3 GetSpecularColor();

void main()
{	
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	// Phase 1: Directional lighting	
	vec3 result = CalcDirLight(dirLight, norm, viewDir);	
	// Phase 2: Point lights
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
	result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	// Phase 3: Spot light
	//result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
	color = vec4(result, material.transparent);	
	//color = vec4(material.useAmbientMap,material.useDiffuseMap,material.useSpecularMap, 1.0);
	//color = vec4(material.specularColor, 1.0f);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// Diffuse shading
	float diff = max(dot(normal, lightDir), 0.0f);
	// Specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// Combine results
	vec3 ambient_color = GetAmbientColor();
	vec3 diffuse_color = GetDiffuseColor();
	vec3 specular_color = GetSpecularColor();	
	
	vec3 ambient = light.ambient * ambient_color;
	vec3 diffuse = light.diffuse * diff * diffuse_color;
	vec3 specular = light.specular * spec * specular_color;
	return (ambient + diffuse + specular);	
}

// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// Diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// Specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// Attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// Combine results
	vec3 ambient_color = GetAmbientColor();
	vec3 diffuse_color = GetDiffuseColor();
	vec3 specular_color = GetSpecularColor();	
	
	vec3 ambient = light.ambient * ambient_color;
	vec3 diffuse = light.diffuse * diff * diffuse_color;
	vec3 specular = light.specular * spec * specular_color;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);	
}

vec3 GetAmbientColor()
{
	vec3 result;
	if(material.useAmbientMap)
		result = vec3(texture(material.ambient, TexCoord));
	else
		result = material.ambientColor;
	return result;
}

vec3 GetDiffuseColor()
{
	vec3 result;
	if(material.useDiffuseMap)
		result = vec3(texture(material.diffuse, TexCoord));
	else
		result = material.diffuseColor;
	return result;
}

vec3 GetSpecularColor()
{
	vec3 result;
	if(material.useSpecularMap)
		result = vec3(texture(material.specular, TexCoord));
	else
		result = material.specularColor;
	return result;
}

