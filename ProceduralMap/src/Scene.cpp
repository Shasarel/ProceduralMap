#include "Scene.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Logger.h"

Scene::Scene(const float windowRatio):
	map(nullptr)
{
	Logger::printInfo("Creating scene");
	projectionMatrix = glm::perspective(glm::radians(75.0f), windowRatio, 0.1f, 5000.0f);

}

Scene::~Scene()
{
	if (map != nullptr) 
		delete map;
}

void Scene::loadMap(const unsigned int chunkResolution, const unsigned int chunkCount, const float mapSize, const float mapHeight)
{
	if (map != nullptr)
		delete map;
	map = new Map(chunkResolution, chunkCount, mapSize, mapHeight);
	skybox = new Skybox();
}

void Scene::render(Camera* camera)
{
	glm::vec3 cameraPosition(camera->x, camera->y, camera->z);
	glm::mat4 rotateMatrix = glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(camera->pitch), glm::vec3(1.0f, 0.0f, 0.0f)), glm::radians(camera->yaw), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0), -cameraPosition);

	glm::mat4 cameraSpaceMatrix = rotateMatrix * translationMatrix;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skybox->render(projectionMatrix, rotateMatrix);

	if(map != nullptr)
		map->renderMap(projectionMatrix, cameraSpaceMatrix, cameraPosition, GL_TRIANGLES);
}
