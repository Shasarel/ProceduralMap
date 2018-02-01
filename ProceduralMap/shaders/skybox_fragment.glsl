#version 330
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, vec3(TexCoords.x, TexCoords.z, TexCoords.y));
}