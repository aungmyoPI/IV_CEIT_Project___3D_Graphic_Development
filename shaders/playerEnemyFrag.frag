#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform vec3 material_ambient;
uniform vec3 material_diffuse;
uniform vec3 material_specular;
uniform float material_shininess;
uniform float material_alpha;

uniform float fogDensity;
uniform vec3 fogColor;

uniform bool hasDiffuseTexture;
uniform sampler2D texture_diffuse1;

void main()
{
	//ambient
	float ambientStrength = 0.1;
	vec3 ambient =  ambientStrength * lightColor;

	//diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// specular
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 objectColor;
	if(hasDiffuseTexture){
		objectColor = texture(texture_diffuse1, TexCoords).rgb;
	}
	else{
		objectColor = material_diffuse;
	}
	vec3 result = (ambient + diffuse + specular) * objectColor;

	float fogDistance = length(viewPos - FragPos);
	float fogAmount = 1.0 - exp(-fogDistance * fogDensity);
	result = mix(result, fogColor, fogAmount);

	FragColor = vec4(result, material_alpha);
}