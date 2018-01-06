#pragma once
class VertexArrayObject {
public:
	VertexArrayObject();
	~VertexArrayObject();

	void createIndices(const unsigned int indicesCount, const unsigned short* indices);
	void deleteIndices();
	void loadIndices(const unsigned int indicesCount, const unsigned int indicesBufferId);
	void activeAttribute(const unsigned int location, const unsigned int count, const void* bufferOffset);
	void setData(const unsigned int size, const float* data);
	unsigned int getIndicesBufferId();
	unsigned int getIndicesCount();
	void render(const unsigned int mode);

private:
	unsigned int objectId, attributesBufferId, indicesBufferId, indicesCount;
};