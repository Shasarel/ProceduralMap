#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normalVector;
layout (location = 2) in vec2 texCoord;

uniform mat4 cameraSpaceMatrix;
uniform mat4 projectionMatrix;
out vec2 fragTexCoord;
out vec3 fragNormalVector;
smooth out vec3 fragPosition;
out float visibility;

const float fogDensity = 0.0004;
const float gradient = 3;

void main(){

	vec4 positionToCamera = cameraSpaceMatrix * vec4(position,1.0);
    gl_Position = projectionMatrix * positionToCamera;
	
	float distanceToVertex = length(positionToCamera);
	visibility = exp(-pow((distanceToVertex*fogDensity),gradient));
	fragTexCoord = texCoord;
	
	fragNormalVector = normalVector;
	fragPosition =  position;
}

