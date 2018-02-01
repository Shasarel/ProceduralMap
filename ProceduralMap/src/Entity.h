#pragma once
#include "VertexArrayObject.h"
class Entity {
public:
	Entity(const float* data, const unsigned int vertexCount, const unsigned int indicesBufferId,const unsigned int indicesCount);
	~Entity();
	void render(const unsigned int mode);
	static void init();
	static void free();
private:
	VertexArrayObject* object;
	VertexArrayObject* baundingBox;
	static unsigned int boxIndicesBufferId;
};