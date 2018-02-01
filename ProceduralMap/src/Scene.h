#pragma once
#include "Camera.h"
#include <glm\glm.hpp>
#include "Map.h"
#include "Skybox.h"
class Scene{
public:
	Scene(const float windowRatio);
	~Scene();
	void loadMap(const unsigned int chunkResolution, const unsigned int chunkCount, const float mapSize, const float mapHeight);
	void render(Camera* camera);
private:
	glm::mat4 projectionMatrix;
	Map* map;
	Skybox* skybox;
};