#pragma once
class VertexArrayObject {
public:
	VertexArrayObject();
	~VertexArrayObject();

	static unsigned int createIndicesBuffer(const unsigned int indicesCount, const unsigned short* indices);
	static void deleteIndicesBuffer(const unsigned int indicesBufferId);
	void loadIndices(const unsigned int indicesCount, const unsigned int indicesBufferId);
	void activeAttribute(const unsigned int location, const unsigned int count, const void* bufferOffset);
	void setData(const unsigned int size, const float* data);
	void render(const unsigned int mode);

private:
	unsigned int objectId, attributesBufferId, indicesCount;
};