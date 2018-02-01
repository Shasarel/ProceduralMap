#pragma once
#include <string>
#include <map>
class TextureManager {
public:
	static TextureManager* getTextureManager();
	static void deleteTextureManager();
	static void flipImage(void* image, int w, int h, int size);

	~TextureManager();
	void addTexture(const std::string textureName, const std::string textureLocation);
	void addCubeTexture(const std::string textureName, const std::string textureLocation[6]);
	void addFontTexture(const std::string textureName, const unsigned int width, const unsigned int height, const unsigned char* data);
	void deleteTexture(const std::string textureName);
	void activeTexture(const std::string textureName, const unsigned int textureType = 0x0DE1);
	void deactiveTexture(const std::string textureName);
	int getTextureActiveSlot(const std::string textureName);

private:
	static TextureManager* instance;
	TextureManager();
	std::map<std::string, unsigned int> textures;
	unsigned int textureActiveSlots[31];

	int getTextureLocation(const std::string textureName);
	int getActiveSlot(const std::string textureName);
};