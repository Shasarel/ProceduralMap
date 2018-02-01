#pragma once
#include "VertexArrayObject.h"
#include "SimplexNoise.h"
#include <glm/glm.hpp>
#include "Shader.h"
#include <thread>
#include <condition_variable>
#include <mutex>
#include "Entity.h"

class Map {
public:
	Map(const unsigned int chunkResolution, const unsigned int chunkCount, const float mapSize, const float mapHeight);
	~Map();
	void renderMap(const glm::mat4 projectionMatrix, const glm::mat4 cameraSpaceMatrix, const glm::vec3 viewerPosition, const unsigned int mode);

private:
	std::thread mapThread;
	std::condition_variable cv;
	std::mutex mtx;
	Entity*** chunks;
	SimplexNoise* noise;
	Shader* shader;
	unsigned int chunkResolution;
	const unsigned int chunkCount, mapLength;
	const unsigned indicesCount;
	const float mapSize, mapHeight;

	bool runThread;
	bool expansionAvailable;
	bool threadExit;
	bool dataReady;
	int expandDirection;
	unsigned int indicesBufferId;
	int offsetX, offsetY;
	float** heightMap;
	float** waterMap;
	float** oldHeightMap;
	float* textureCoords;
	float** dataToVao;

	void hydrologicErosion();
	void mapThreadLoop();
	void checkExpansion(glm::vec3 viewerPosition);
	void checkNewData();
	void expandMap();

	Entity* createChunk(const float* data) const;
	float* createChunkData(const unsigned int x, const unsigned int y) const;
	glm::vec3 getNormalVector(const int i, const int j) const;
};