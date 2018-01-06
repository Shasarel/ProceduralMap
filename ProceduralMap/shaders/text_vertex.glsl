#version 330 core


layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

uniform vec2 translation;
uniform vec2 scale;

out vec2 fragTexCoord;

void main(void) {
  gl_Position = vec4((position * scale) + translation, 0, 1);
  fragTexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
}

