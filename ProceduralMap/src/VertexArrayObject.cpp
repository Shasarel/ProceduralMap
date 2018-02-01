#include "VertexArrayObject.h"
#include <GL/glew.h>

VertexArrayObject::VertexArrayObject()
{
	glGenBuffers(1, &attributesBufferId);
	glGenVertexArrays(1, &objectId);
	glBindVertexArray(objectId);
		glBindBuffer(GL_ARRAY_BUFFER, attributesBufferId);
	glBindVertexArray(0);
}

VertexArrayObject::~VertexArrayObject()
{
	glDeleteBuffers(1, &attributesBufferId);
	glDeleteVertexArrays(1, &objectId);
}

unsigned int VertexArrayObject::createIndicesBuffer(const unsigned int indicesCount, const unsigned short* indices)
{
	unsigned int indicesBufferId = 0;
	glGenBuffers(1, &indicesBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(unsigned short), indices, GL_STATIC_DRAW);
	return indicesBufferId;
}

void VertexArrayObject::deleteIndicesBuffer(const unsigned int indicesBufferId)
{
	glDeleteBuffers(1, &indicesBufferId);
}

void VertexArrayObject::loadIndices(const unsigned int indicesCount, const unsigned int indicesBufferId)
{
	glBindVertexArray(objectId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBufferId);
	glBindVertexArray(0);
	this->indicesCount = indicesCount;
}

void VertexArrayObject::activeAttribute(const unsigned int location, const unsigned int count, const void* bufferOffset)
{
	glBindVertexArray(objectId);
		glBindBuffer(GL_ARRAY_BUFFER, attributesBufferId);
		glEnableVertexAttribArray(location);
		glVertexAttribPointer(location, count, GL_FLOAT, GL_FALSE, 0, bufferOffset);
	glBindVertexArray(0);
}

void VertexArrayObject::setData(const unsigned int size, const float * data)
{
	glBindBuffer(GL_ARRAY_BUFFER, attributesBufferId);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}


void VertexArrayObject::render(const unsigned int mode)
{
	glBindVertexArray(objectId);
		glDrawElements(mode, indicesCount, GL_UNSIGNED_SHORT, (void*)0);
	glBindVertexArray(0);
}
