#version 330 core


in vec2 fragTexCoord;
uniform sampler2D tex;

out vec4 outputColor;

void main()
{
	outputColor = vec4(0, 0, 0, texture(tex, fragTexCoord).r);
}