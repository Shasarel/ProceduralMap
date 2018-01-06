#pragma once
#include <thread>
#include "SimplexNoise.h"
#include <mutex>
#include <condition_variable>
class MapManager {

private:
	std::thread mapThread;
	std::condition_variable cv;
	std::mutex mtx;
	SimplexNoise * sn;

	int fragmentResolution, fragmentCount, mapLength;;
	const float mapSize, mapHeight;

	int mapExpandOffsetX, mapExpandOffsetY;
	float ** heighMap, ** oldHeighMap;

	float * dataOut;
	bool needUpdate, newDataReady, _exit;

	void updateData();
	void mapMainLoop();
	void expandMap();
	glm::vec3 getNormalVector(int i, int j);

public:
	int expandDirection;
	MapManager(int fragmentResolution, int fragmentCount, float mapSize, float mapHeight);
	~MapManager();

	void expandMap(int direction);
	float * getData();
	float getHeightAt(float x, float y);
	bool isNewDataReady();
	void freeOldHeightMap();
};