#pragma once
#include "VertexArrayObject.h"
#include "Shader.h"
#include "Camera.h"
class Skybox {
public:
	Skybox();
	~Skybox();
	void render(const glm::mat4 projectionMatrix, const glm::mat4 rotationMatrix);
private:
	VertexArrayObject * vao;
	Shader* shader;
};