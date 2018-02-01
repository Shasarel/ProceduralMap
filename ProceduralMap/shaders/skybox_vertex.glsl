#version 330 core
layout (location = 0) in vec3 position;

out vec3 TexCoords;

uniform mat4 projectionMatrix;
uniform mat4 cameraSpaceMatrix;

void main()
{
    TexCoords = position;
    gl_Position = projectionMatrix * cameraSpaceMatrix * vec4(position, 1.0);
}  