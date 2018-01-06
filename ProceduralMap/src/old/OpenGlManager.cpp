#include <string>
#include <vector>
#include <fstream>
#include <GL/glew.h>
#include "OpenGLManager.h"
#include <stb_image.h>

OpenGLManager::OpenGLManager(int mapVertices, float monitorRatio)
{
	projectionMatrix = glm::perspective(glm::radians(75.0f), monitorRatio, 0.1f, 5000.0f);

	map = new Map(219, 10, 1.0f, 150);

}

OpenGLManager::~OpenGLManager()
{
	delete map;
	delete textRenderer;
	TextureManager::deleteTextureManager();
}



void OpenGLManager::renderMap(glm::vec3 viewerPosition, float pitch, float yaw)
{

	glm::mat4 rotateMatrix = glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f)), glm::radians(yaw), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0), -viewerPosition);

	glm::mat4 cameraSpaceMatrix = rotateMatrix * translationMatrix;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		map->renderMap(projectionMatrix ,cameraSpaceMatrix, viewerPosition, GL_TRIANGLES);
}
