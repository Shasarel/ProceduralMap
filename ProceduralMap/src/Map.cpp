#include "Map.h"
#include <glm/gtc/matrix_transform.hpp>
#include "TextureManager.h"
#include "Logger.h"

Map::Map(const unsigned int chunkResolution, const unsigned int chunkCount, const float mapSize, const float mapHeight) :
	chunkResolution(chunkResolution),
	chunkCount(chunkCount),
	mapSize(mapSize),
	mapHeight(mapHeight),
	indicesCount((chunkResolution - 1)*(chunkResolution - 1) * 6),
	mapLength(chunkCount * chunkResolution - chunkCount + 1),
	offsetX(0),
	offsetY(0),
	threadExit(false),
	runThread(false),
	dataReady(false),
	expandDirection(-1),
	expansionAvailable(true)
{
	Entity::init();
	Logger::printInfo("Creating map");
	if (chunkResolution > 255) {
		Logger::printWarning("Chunk resolution too big. Set to 255");
		this->chunkResolution = 255;
	}
	noise = new SimplexNoise(333, 10, 0.5);
	shader = new Shader("shaders/map_vertex.glsl", "shaders/map_fragment.glsl");

	unsigned short* indices = new unsigned short[indicesCount];
	int n = 0;
	for (unsigned int i = 0; i < chunkResolution - 1; i++) {
		for (unsigned int j = 0; j < chunkResolution - 1; j++) {
			indices[n++] = i * chunkResolution + j;
			indices[n++] = i * chunkResolution + (j + 1);
			indices[n++] = (i + 1) * chunkResolution + j;
			indices[n++] = i * chunkResolution + (j + 1);
			indices[n++] = (i + 1) * chunkResolution + (j + 1);
			indices[n++] = (i + 1) * chunkResolution + j;
		}
	}
	indicesBufferId = VertexArrayObject::createIndicesBuffer(indicesCount, indices);

	dataToVao = new float*[chunkCount];

	chunks = new Entity**[chunkCount];
	for (unsigned int i = 0;i < chunkCount;i++) {
		chunks[i] = new Entity*[chunkCount];
	}
	heightMap = new float*[mapLength];
	for (unsigned int i = 0; i < mapLength; i++) {
		heightMap[i] = new float[mapLength];
		for (unsigned int j = 0; j < mapLength; j++) {
			heightMap[i][j] = (float)noise->simplexNoise(i, j) * mapHeight;
		}
	}
	n = 0;
	textureCoords = new float[chunkResolution * chunkResolution * 3];
	for (unsigned int i = 0;i < chunkResolution;i++) {
		for (unsigned int j = 0;j < chunkResolution;j++) {
			textureCoords[n++] = j * (float)(70) / chunkResolution;
			textureCoords[n++] = i * (float)(70) / chunkResolution;
		}
	}
	//Logger::printInfo("Hydrologic erosion.");
	//hydrologicErosion();
	Logger::printInfo("Loading starting map.");
	for (unsigned int i = 0;i < chunkCount;i++)
		for (unsigned int j = 0;j < chunkCount;j++)
			chunks[i][j] = createChunk(createChunkData(i, j));

	TextureManager::getTextureManager()->addTexture("rock", "textures/rock.jpg");
	TextureManager::getTextureManager()->addTexture("grass", "textures/grass.jpg");
	TextureManager::getTextureManager()->addTexture("snow", "textures/snow.jpg");

	TextureManager::getTextureManager()->activeTexture("rock");
	TextureManager::getTextureManager()->activeTexture("grass");
	TextureManager::getTextureManager()->activeTexture("snow");

	mapThread = std::thread(&Map::mapThreadLoop, this);
}

Map::~Map()
{
	threadExit = true;
	runThread = true;
	cv.notify_one();
	mapThread.join();

	TextureManager::getTextureManager()->deleteTexture("rock");
	TextureManager::getTextureManager()->deleteTexture("grass");
	TextureManager::getTextureManager()->deleteTexture("snow");
	
	delete shader;
	delete[] textureCoords;

	VertexArrayObject::deleteIndicesBuffer(indicesBufferId);
	for (unsigned int i = 0;i < chunkCount;i++) {
		for (unsigned int j = 0;j < chunkCount;j++) {
			delete chunks[i][j];
		}
		delete[] chunks[i];
	}
	delete[] chunks;
	for (unsigned int i = 0; i < mapLength; i++) {
		delete[] heightMap[i];
	}
	delete[] heightMap;
	delete noise;
	delete[] dataToVao;
}

void Map::hydrologicErosion()
{
}

void Map::mapThreadLoop()
{
	while (!threadExit) 
	{
		expandMap();
		std::unique_lock<std::mutex> lck(mtx);
		cv.wait(lck, [this] { return runThread; });

		runThread = false;
	}
}

void Map::checkExpansion(glm::vec3 viewerPosition)
{
	if (expansionAvailable) {
		if (viewerPosition.x - chunkResolution * mapSize * (offsetX + 0.5f) > 0.0f) {
			expandDirection = 4;
			offsetX++;
			
			runThread = true;
			expansionAvailable = false;
			cv.notify_one();
		}
		else if (viewerPosition.x - chunkResolution * mapSize * (offsetX - 0.5f) < 0.0f) {
			expandDirection = 2;
			offsetX--;

			runThread = true;
			expansionAvailable = false;
			cv.notify_one();
		}
		else if (viewerPosition.y - chunkResolution * mapSize * (offsetY + 0.5f) > 0.0f) {
			expandDirection = 3;
			offsetY++;

			runThread = true;
			expansionAvailable = false;
			cv.notify_one();
		}
		else if (viewerPosition.y - chunkResolution * mapSize * (offsetY - 0.5f) < 0.0f) {
			expandDirection = 1;
			offsetY--;

			runThread = true;
			expansionAvailable = false;
			cv.notify_one();
		}
	}
}

void Map::checkNewData()
{
	if (dataReady) {
		Logger::printInfo("Loading expanded map");
		if (expandDirection == 3) {
		for (unsigned int i = 0;i < chunkCount;i++) {
			delete chunks[i][0];
			for (unsigned int j = 1;j < chunkCount;j++) {
				chunks[i][j - 1] = chunks[i][j];
			}
			chunks[i][chunkCount -1] = createChunk(dataToVao[i]);
			}
		}
		else if (expandDirection == 4) {
			for (unsigned int i = 0;i < chunkCount;i++) {
				delete chunks[0][i];
				for (unsigned int j = 1;j < chunkCount;j++) {
					chunks[j - 1][i] = chunks[j][i];
				}
				chunks[chunkCount - 1][i] = createChunk(dataToVao[i]);
			}
		}
		else if (expandDirection == 1) {
			for (unsigned int i = 0;i < chunkCount;i++) {
				delete chunks[i][chunkCount - 1];
			for (unsigned int j = chunkCount -1 ;j > 0;j--) {
					chunks[i][j] = chunks[i][j - 1];
				}
				chunks[i][0] = createChunk(dataToVao[i]);
			}
		}
		 else if (expandDirection == 2) {
			for (unsigned int i = 0;i < chunkCount;i++) {
				delete chunks[chunkCount - 1][i];
				for (unsigned int j = chunkCount - 1;j > 0;j--) {
					chunks[j][i] = chunks[j - 1][i];
				}
				chunks[0][i] = createChunk(dataToVao[i]);
			}
		}
		expansionAvailable = true;
		dataReady = false;
		Logger::printInfo("Map loaded");
	}
}

Entity* Map::createChunk(const float* data) const
{
	return new Entity(data, chunkResolution * chunkResolution, indicesBufferId, indicesCount);
}

float* Map::createChunkData(const unsigned int x, const unsigned int y) const
{
	unsigned int chunkVertices = chunkResolution * chunkResolution;

	int vertexLocationX = (chunkResolution - 1) * x;
	int vertexLocationY = (chunkResolution - 1) * y;

	int vertexLocationOffsetX = ((int)chunkResolution - 1) * offsetX;
	int vertexLocationOffsetY = ((int)chunkResolution - 1) * offsetY;

	float centerMapOffset = (mapLength >> 1) * mapSize;

	unsigned int n = 0, m = 0;

	float* data = new float[chunkVertices * 8];
	for (unsigned int i = 0;i < chunkResolution;i++) {
		for (unsigned int j = 0;j < chunkResolution;j++) {
			data[n++] = ((vertexLocationX + vertexLocationOffsetX + (int)i) * mapSize) - centerMapOffset;
			data[n++] = ((vertexLocationY + vertexLocationOffsetY + (int)j) * mapSize) - centerMapOffset;
			
			data[n++] = heightMap[vertexLocationX + i][vertexLocationY + j];

			glm::vec3 normal = getNormalVector(vertexLocationX + i, vertexLocationY + j);
			data[chunkVertices * 3 + m++] = normal.x;
			data[chunkVertices * 3 + m++] = normal.y;
			data[chunkVertices * 3 + m++] = normal.z;

		}
	}

	memcpy(&data[chunkVertices * 6], textureCoords, chunkResolution * chunkResolution * 2 * sizeof(float));
	return data;
}

glm::vec3 Map::getNormalVector(const int i, const int j) const
{

	float* normalHeight = new float[6];

	glm::vec3 normalVector;
	if (i == 0) {
		normalHeight[4] = (float)noise->simplexNoise(i - 1 + (chunkResolution - 1.0f) * offsetX, j  + (chunkResolution - 1.0f) * offsetY) * mapHeight;
	}
	else {
		normalHeight[4] = heightMap[i - 1][j];
	}

	if (i == mapLength - 1) {
		normalHeight[5] = (float)noise->simplexNoise(i + 1 + (chunkResolution - 1.0f) * offsetX, j + (chunkResolution - 1.0f) * offsetY) * mapHeight;
	}
	else {
		normalHeight[5] = heightMap[i + 1][j];
	}

	if (j == 0) {
		normalHeight[2] = (float)noise->simplexNoise(i + (chunkResolution - 1.0f) * offsetX, j - 1 + (chunkResolution - 1.0f) * offsetY) * mapHeight;
	}
	else {
		normalHeight[2] = heightMap[i][j - 1];
	}

	if (j == mapLength - 1) {
		normalHeight[0] = (float)noise->simplexNoise(i + (chunkResolution - 1.0f) * offsetX, j + 1 + (chunkResolution - 1.0f) * offsetY) * mapHeight;
	}
	else {
		normalHeight[0] = heightMap[i][j + 1];
	}


	if (j == 0 || i == mapLength - 1) {
		normalHeight[3] = (float)noise->simplexNoise(i + 1 + (chunkResolution - 1.0f) * offsetX, j - 1 + (chunkResolution - 1.0f) * offsetY) * mapHeight;
	}
	else {
		normalHeight[3] = heightMap[i + 1][j - 1];
	}

	if (j == mapLength - 1 || i == 0) {
		normalHeight[1] = (float)noise->simplexNoise(i - 1 + (chunkResolution - 1.0f) * offsetX, j + 1 + (chunkResolution - 1.0f) * offsetY) * mapHeight;
	}
	else {
		normalHeight[1] = heightMap[i - 1][j + 1];
	}

	float y =  (2 * (normalHeight[2] - normalHeight[0]) - normalHeight[1] + normalHeight[3] + normalHeight[4] - normalHeight[5]) / mapSize;
	float x = -(2 * (normalHeight[5] - normalHeight[4]) + normalHeight[1] + normalHeight[3] - normalHeight[5] - normalHeight[2]) / mapSize;
	float z = 6;
	normalVector = glm::normalize(glm::vec3(x, y, z));

	delete[] normalHeight;
	return normalVector;;
}

void Map::renderMap(const glm::mat4 projectionMatrix, const glm::mat4 cameraSpaceMatrix, const glm::vec3 viewerPosition, const unsigned int mode)
{
	shader->use();
	checkExpansion(viewerPosition);
	checkNewData();
	shader->setUniformValue("cameraSpaceMatrix", cameraSpaceMatrix);
	shader->setUniformValue("projectionMatrix", projectionMatrix);
	shader->setUniformValue("viewerPosition", viewerPosition);
	shader->setUniformValue("mapHeight", mapHeight);
	shader->setUniformValue("grass", TextureManager::getTextureManager()->getTextureActiveSlot("grass"));
	shader->setUniformValue("rock", TextureManager::getTextureManager()->getTextureActiveSlot("rock"));
	shader->setUniformValue("snow", TextureManager::getTextureManager()->getTextureActiveSlot("snow"));
	for (unsigned int i = 0;i < chunkCount;i++)
		for (unsigned int j = 0;j < chunkCount;j++)
			chunks[i][j]->render(mode);

}
void Map::expandMap()
{
	/*float ** oldHeighMap = new float*[mapLength];
	for (int i = 0; i < mapLength; i++) {
		oldHeighMap[i] = new float[mapLength];
		for (int j = 0; j < mapLength; j++) {
			oldHeighMap[i][j] = heightMap[i][j];
		}
	}
	this->oldHeightMap = oldHeighMap;*/
	if(expandDirection != -1)
		Logger::printInfo("Map expanding: " + std::to_string(expandDirection));
	if (expandDirection == 3) {
		for (unsigned int i = 0; i < mapLength; i++) {
			for (unsigned int j = chunkResolution; j < mapLength; j++) {
				heightMap[i][j - (chunkResolution)] = heightMap[i][j];
			}
			for (unsigned int j = mapLength - chunkResolution - 1; j < mapLength ; j++) {
				heightMap[i][j] = (float)noise->simplexNoise(i + (chunkResolution - 1.0f) * offsetX, j + (chunkResolution - 1.0f) * offsetY) * mapHeight;
			}
		}
		for (unsigned int i = 0;i < chunkCount;i++) {
			dataToVao[i] = createChunkData(i, chunkCount - 1);
		}
		dataReady = true;
	}
	else if (expandDirection == 4) {
		for (unsigned int i = 0; i < mapLength; i++) {
			for (unsigned int j = chunkResolution; j < mapLength; j++) {
				heightMap[j - chunkResolution][i] = heightMap[j][i];
			}
			for (unsigned int j = mapLength - chunkResolution - 1; j < mapLength; j++) {
				heightMap[j][i] = (float)noise->simplexNoise(j + (chunkResolution - 1.0f) * offsetX, i + (chunkResolution - 1.0f) * offsetY) * mapHeight;
			}
		}
		for (unsigned int i = 0;i < chunkCount;i++) {
			dataToVao[i] = createChunkData(chunkCount - 1, i);
		}
		dataReady = true;
	}
	else if (expandDirection == 1) {
		for (unsigned int i = 0; i < mapLength; i++) {
			for (unsigned int j = mapLength - 1; j >= chunkResolution; j--) {
				heightMap[i][j] = heightMap[i][j - chunkResolution];
			}
			for (unsigned int j = 0; j < chunkResolution + 1; j++) {
				heightMap[i][j] = (float)noise->simplexNoise(i + (chunkResolution  - 1.0f)* offsetX, j + (chunkResolution - 1.0f) * offsetY) * mapHeight;
			}
		}
		for (unsigned int i = 0;i < chunkCount;i++) {
			dataToVao[i] = createChunkData(i, 0);
		}
		dataReady = true;
	}
	else if (expandDirection == 2) {
		for (unsigned int i = 0; i < mapLength; i++) {
			for (unsigned int j = mapLength - 1; j >= chunkResolution; j--) {
				heightMap[j][i] = heightMap[j - chunkResolution][i];
			}
			for (unsigned int j = 0; j < chunkResolution + 1; j++) {
				heightMap[j][i] = (float)noise->simplexNoise(j + (chunkResolution - 1.0f) * offsetX, i + (chunkResolution - 1.0f) * offsetY) * mapHeight;
			}
		}
		for (unsigned int i = 0;i < chunkCount;i++) {
			dataToVao[i] = createChunkData(0, i);
		}
		dataReady = true;
	}
}