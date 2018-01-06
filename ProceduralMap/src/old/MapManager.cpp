#include <glm/glm.hpp>
#include "MapManager.h"
#include "VertexArrayObject.h"

//#include <stdio.h>
//#include <chrono>

MapManager::MapManager(int fragmentResolution, int fragmentCount, float mapSize, float mapHeight) :
	fragmentResolution(fragmentResolution),
	fragmentCount(fragmentCount),
	mapSize(mapSize),
	mapHeight(mapHeight),
	mapLength(fragmentCount * fragmentResolution),
	needUpdate(false),
	newDataReady(false),
	_exit(false),
	mapExpandOffsetX(0),
	mapExpandOffsetY(0),
	expandDirection(-1)
{
	sn = new SimplexNoise(333, 10, 0.5);

	heighMap = new float*[mapLength];
	oldHeighMap = heighMap;
	dataOut = new float[mapLength * mapLength * 8];

	for (int i = 0; i < mapLength; i++) {
		heighMap[i] = new float[mapLength];
		for (int j = 0; j < mapLength; j++) {
			int index = 3 * (i * mapLength + j);
			*(dataOut + index + 0) = (i - (mapLength >> 1)) * mapSize;
			*(dataOut + index + 1) = (j - (mapLength >> 1)) * mapSize;

			int indexTexture = 2 * (i * mapLength + j) + mapLength * 6 * mapLength;
			*(dataOut + indexTexture + 0) = j * (float)(70) / fragmentResolution;
			*(dataOut + indexTexture + 1) = i * (float)(70) / fragmentResolution;

			heighMap[i][j] = (float)sn->simplexNoise(i, j) * mapHeight;
		}
	}
	mapThread = std::thread(&MapManager::mapMainLoop, this);
}

MapManager::~MapManager()
{
	_exit = true;
	needUpdate = true;
	cv.notify_one();
	mapThread.join();

	for (int i = 0; i < mapLength; i++)
		delete[] heighMap[i];
	delete[] heighMap;

	delete sn;
	delete[] dataOut;
}

//1,2,3,4, - kierunki , inne - zwykły update
void MapManager::expandMap(int direction)
{
	expandDirection = direction;
	needUpdate = true;
	cv.notify_one();
}

float * MapManager::getData()
{
	newDataReady = false;
	return dataOut;
}

float MapManager::getHeightAt(float xPos, float yPos)
{
	float x = (xPos / mapSize) + (fragmentResolution * fragmentCount / 2.0f);
	float y = (yPos / mapSize) + (fragmentResolution * fragmentCount / 2.0f);


	int xInt = (int)(x);
	int yInt = (int)(y);

	if (xInt == mapLength)
		xInt--;
	if (yInt == mapLength)
		yInt--;
	float D1 = sqrt(pow(xInt - x, 2) + pow(yInt - y, 2));
	float D2 = sqrt(pow(xInt + 1 - x, 2) + pow(yInt + 1 - y, 2));

	glm::vec3 P1;
	glm::vec3 P2 = glm::vec3(xInt, yInt + 1, oldHeighMap[xInt][yInt + 1]);
	glm::vec3 P3 = glm::vec3(xInt + 1, yInt, oldHeighMap[xInt + 1][yInt]);

	if (D1 < D2)
		P1 = glm::vec3(xInt, yInt, oldHeighMap[xInt][yInt]);
	else 
		P1 = glm::vec3(xInt + 1, yInt + 1, oldHeighMap[xInt + 1][yInt + 1]);

	glm::vec3 params = glm::cross((P2 - P1), (P3 - P1));
	float D = -(params.x*P1.x + params.y * P1.y + params.z *P1.z);


	float z = (-D - params.x * x - params.y * y) / params.z;
	return z;
}

bool MapManager::isNewDataReady()
{
	return newDataReady;
}

void MapManager::freeOldHeightMap()
{
	for (int i = 0; i < mapLength; i++)
		delete[] oldHeighMap[i];
	delete[] oldHeighMap;
	oldHeighMap = heighMap;
}

glm::vec3 MapManager::getNormalVector(int i, int j)
{
	glm::vec3 normalVector;
	if (i == 0 || i == mapLength - 1 || j == 0 || j == mapLength - 1) {
		normalVector = glm::vec3(0.0f, 0.0f, 1.0f);
	}
	else {
		float y = (2 * (heighMap[i][j - 1] - heighMap[i][j + 1]) - heighMap[i - 1][j + 1] + heighMap[i + 1][j - 1] + heighMap[i - 1][j] - heighMap[i + 1][j]) / mapSize;
		float x = -(2 * (heighMap[i + 1][j] - heighMap[i - 1][j]) + heighMap[i - 1][j + 1] + heighMap[i + 1][j - 1] - heighMap[i - 1][j] - heighMap[i][j - 1]) / mapSize;
		float z = 6;
		normalVector = glm::normalize(glm::vec3(x, y, z));
	}
	return normalVector;
}

void MapManager::updateData()
{
	int index, indexNormal;

	for (int i = 0; i < mapLength; i++) 
	{
		for (int j = 0; j < mapLength; j++) 
		{
			index = 3 * (i * mapLength + j);
			*(dataOut + index + 2) = heighMap[i][j];

			glm::vec3 normalVector = getNormalVector(i, j);
			indexNormal = index + mapLength * 3 * mapLength;
			*(dataOut + indexNormal + 0) = normalVector.x;
			*(dataOut + indexNormal + 1) = normalVector.y;
			*(dataOut + indexNormal + 2) = normalVector.z;


		}
	}
	newDataReady = true;
}

void MapManager::mapMainLoop()
{

	while (!_exit) {

		auto now = std::chrono::system_clock::now();
		expandMap();
		std::chrono::duration<double> diff = std::chrono::system_clock::now() - now;
		double dt = diff.count();
		printf("Czas generowania nowego terenu: %f\n", dt);

		now = std::chrono::system_clock::now();
		updateData();
		diff = std::chrono::system_clock::now() - now;
		dt = diff.count();
		printf("Czas ladowaia danych: %f\n", dt);

		std::unique_lock<std::mutex> lck(mtx);
		cv.wait(lck, [this] { return needUpdate; });

		needUpdate = false;
	}

}

void MapManager::expandMap()
{
	float ** oldHeighMap = new float*[mapLength];
	for (int i = 0; i < mapLength; i++) {
		oldHeighMap[i] = new float[mapLength];
		for (int j = 0; j < mapLength; j++) {
			oldHeighMap[i][j] = heighMap[i][j];
		}
	}
	this->oldHeighMap = oldHeighMap;
	if (expandDirection == 1) {
		mapExpandOffsetY--;
		for (int i = 0; i < mapLength; i++) {
			for (int j = fragmentResolution; j < mapLength; j++) {
				heighMap[i][j - fragmentResolution] = heighMap[i][j];
			}
			for (int j = mapLength - fragmentResolution; j < mapLength; j++) {
				heighMap[i][j] = (float)sn->simplexNoise(i - fragmentResolution * mapExpandOffsetX, j - fragmentResolution * mapExpandOffsetY) * mapHeight;
			}
		}
	}
	else if (expandDirection == 2) {
		mapExpandOffsetX--;
		for (int i = 0; i < mapLength; i++) {
			for (int j = fragmentResolution; j < mapLength; j++) {
				heighMap[j - fragmentResolution][i] = heighMap[j][i];
			}
			for (int j = mapLength - fragmentResolution; j < mapLength; j++) {
				heighMap[j][i] = (float)sn->simplexNoise(j - fragmentResolution * mapExpandOffsetX, i - fragmentResolution * mapExpandOffsetY) * mapHeight;
			}
		}
	}
	else if (expandDirection == 3) {
		mapExpandOffsetY++;
		for (int i = 0; i < mapLength; i++) {
			for (int j = mapLength - 1; j >= fragmentResolution; j--) {
				heighMap[i][j] = heighMap[i][j - fragmentResolution];
			}
			for (int j = 0; j < fragmentResolution; j++) {
				heighMap[i][j] = (float)sn->simplexNoise(i - fragmentResolution * mapExpandOffsetX, j - fragmentResolution * mapExpandOffsetY) * mapHeight;
			}
		}
	}
	else if (expandDirection == 4) {
		mapExpandOffsetX++;
		for (int i = 0; i < mapLength; i++) {
			for (int j = mapLength - 1; j >= fragmentResolution; j--) {
				heighMap[j][i] = heighMap[j - fragmentResolution][i];
			}
			for (int j = 0; j < fragmentResolution; j++) {
				heighMap[j][i] = (float)sn->simplexNoise(j - fragmentResolution * mapExpandOffsetX, i - fragmentResolution * mapExpandOffsetY) * mapHeight;
			}
		}
	}
}