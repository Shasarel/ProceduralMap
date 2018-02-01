#include "Entity.h"
#include <string.h>
unsigned int Entity::boxIndicesBufferId;
Entity::Entity(const float * data, const unsigned int vertexCount, const unsigned int indicesBufferId, const unsigned int indicesCount)
{
	object = new VertexArrayObject();
	float pos[] = { 1000,1000,1000,
					1050,1000,1000,
					1050,1050,1000,
					1000,1050,1000,
					1000,1000,1050,
					1050,1000,1050,
					1000,1050,1050,
					1050,1050,1050};
	baundingBox = new VertexArrayObject();
	baundingBox->loadIndices(24, boxIndicesBufferId);
	baundingBox->activeAttribute(0, 3, 0);
	baundingBox->setData(8*3 * sizeof(float), pos);
	object->loadIndices(indicesCount, indicesBufferId);
	object->activeAttribute(0, 3, 0);
	object->activeAttribute(1, 3, (void*)(vertexCount * sizeof(float) * 3));
	object->activeAttribute(2, 2, (void*)(vertexCount * sizeof(float) * 6));
	object->setData(vertexCount * 8 * sizeof(float), data);
}

Entity::~Entity()
{
	delete object;
}

void Entity::render(const unsigned int mode)
{
	object->render(mode);

	//baundingBox->render(1);
}

void Entity::init()
{
	unsigned short boxIndices[] = { 0, 1,
									1, 2,
									2, 3,
									3, 0,
									0, 4,
									1, 5,
									2, 7,
									3, 6,
									4, 6,
									6, 7,
									5, 7,
									4, 5};
	boxIndicesBufferId = VertexArrayObject::createIndicesBuffer(24, boxIndices);
}

void Entity::free()
{
	VertexArrayObject::deleteIndicesBuffer(boxIndicesBufferId);
}
