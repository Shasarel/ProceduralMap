#version 330

in vec2 fragTexCoord;
in vec3 fragNormalVector;
in float visibility;
smooth in vec3 fragPosition;

out vec4 outputColor;
uniform vec3 viewerPosition;
uniform float mapHeight;

uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D snow;

vec4 getPhongLight(){
	vec3 lightPosition = vec3(200000,200000,200000);
	vec3 lightColor = vec3(1.0,1.0,1.0);
	
	
	vec3 lightDirection = normalize(lightPosition - fragPosition);
	
	
	float ambientStrength = 0.15;
	vec3 ambient = ambientStrength * lightColor;

	
	float diffuseStrenght = 0.6;
	float diff = clamp(dot(fragNormalVector, lightDirection),0,1);
	vec3 diffuse = diff * lightColor * diffuseStrenght;
	
	float specularStrength = 0.3;
	int shininess = 4;
	vec3 viewDir = normalize(viewerPosition - fragPosition);
	vec3 reflectDir = reflect(-lightDirection, fragNormalVector);
	float spec = pow(clamp(dot(viewDir, reflectDir), 0,1), shininess) * specularStrength;
	
	vec3 finalLight = (ambient + diffuse + spec);
	
	return vec4(finalLight,1.0);
}

float homographicFunction(float x, float move, int power){
	return clamp(1/pow((1 - x + move),power),0,1);
}

vec4 getTexture(){

	float normalTex = homographicFunction(fragNormalVector.z, 0.8, 4);
	float grountHeight = homographicFunction(fragPosition.z / mapHeight,0.5,20);
	vec4 groundTex = mix(texture(grass,fragTexCoord),texture(rock,fragTexCoord),grountHeight);
	
	grountHeight = homographicFunction(fragPosition.z / mapHeight,0.64,20);
	groundTex = mix(groundTex,texture(snow,fragTexCoord), grountHeight);
	
	normalTex = homographicFunction(fragNormalVector.z, 0.8, 20);
	groundTex = mix(texture(rock,fragTexCoord),groundTex, normalTex);
	return groundTex;
}

void main()
{
	
	vec4 light = getPhongLight();
	vec4 tex = getTexture();
	
	vec4 fogColor = vec4(0.5,0.5,0.5,1);
    outputColor = mix(fogColor, light * tex,visibility);
}