#include "TextureManager.h"
#include <GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Logger.h"

TextureManager* TextureManager::instance = new TextureManager();

TextureManager::TextureManager()
{
	for (int i = 0;i < 31;i++)
		textureActiveSlots[i] = 0;
}

TextureManager * TextureManager::getTextureManager()
{
	return instance;
}

void TextureManager::deleteTextureManager()
{
	delete instance;
}

void TextureManager::flipImage(void * image, int w, int h, int size)
{
	stbi__vertical_flip(image, w, h, size);
}

TextureManager::~TextureManager()
{
	for (auto& texture : textures) {
		glDeleteTextures(1, &texture.second);
	}
}

void TextureManager::addTexture(const std::string textureName, const std::string textureLocation)
{
	Logger::printInfo("Loading texture: " + textureName);
	int width, height, nrChannels;
	unsigned char* data = stbi_load(textureLocation.c_str(), &width, &height, &nrChannels, 0);
	unsigned int tex;

	if (data == 0)
	{
		Logger::printError("Unable to load texture: " + textureName + ", location: " +  textureLocation);
	}

	if (getTextureLocation(textureName) != -1) 
	{
		Logger::printWarning("Texture already exist. Overriding: " + textureName);
		deleteTexture(textureName);
	}

	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE31);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	

	stbi_image_free(data);

	textures[textureName] = tex;
}

void TextureManager::addCubeTexture(const std::string textureName, const std::string textureLocation[6])
{
	Logger::printInfo("Loading texture: " + textureName);
	if (getTextureLocation(textureName) != -1)
	{
		Logger::printWarning("Texture already exist. Overriding: " + textureName);
		deleteTexture(textureName);
	}

	unsigned int tex;

	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE31);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	textures[textureName] = tex;

	for (int i = 0; i < 6; i++) {

		int width, height, nrChannels;
		unsigned char* data = stbi_load(textureLocation[i].c_str(), &width, &height, &nrChannels, 0);

		if (data == 0)
		{
			Logger::printError("Unable to load texture: " + textureName + ", location: " + textureLocation[i]);
		}
		else {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}

		stbi_image_free(data);
	}
}

void TextureManager::addFontTexture(const std::string textureName, const unsigned int width, const unsigned int height, const unsigned char* data)
{
	unsigned int tex;

	if (data == 0)
	{
		//printf("Unable to load font texture: %s\n", textureName.c_str());
	}

	if (getTextureLocation(textureName) != -1)
	{
		//printf("Texture already exist. Overriding: %s\n", textureName.c_str());
		deleteTexture(textureName);
	}
	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE31);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);

	textures[textureName] = tex;
}

void TextureManager::deleteTexture(const std::string textureName)
{
	const int textureLocation = getTextureLocation(textureName);
	this->deactiveTexture(textureName);
	if (textureLocation != -1) 
	{
		const unsigned int uTextureLocation = textureLocation;
		glDeleteTextures(1, &uTextureLocation);
	}
}

void TextureManager::activeTexture(const std::string textureName, const unsigned int textureType)
{
	int textureActiveSlot = getActiveSlot(textureName);
	if (textureActiveSlot != -1)
	{
		Logger::printWarning("Texture " + textureName +" is already activated");
	}
	else
	{
		int textureLocation = getTextureLocation(textureName);
		if (textureLocation != -1) {
			for (int i = 0;i < 31;i++)
			{
				if (textureActiveSlots[i] == 0) {
					textureActiveSlots[i] = textureLocation;
					glActiveTexture(GL_TEXTURE0 + i);
					glBindTexture(textureType, textureLocation);
					break;
				}
			}
		}
		else {
			Logger::printWarning("Texture " + textureName + " does not exist");
		}
	}
}

void TextureManager::deactiveTexture(const std::string textureName)
{
	int textureActiveSlot = getActiveSlot(textureName);
	if (textureActiveSlot != -1)
	{
		textureActiveSlots[textureActiveSlot] = 0;
	}
	else
	{
		Logger::printWarning("Texture " + textureName + " is not activated");
	}
}

int TextureManager::getTextureActiveSlot(const std::string textureName)
{
	int textureActiveSlot = getActiveSlot(textureName);
	if (textureActiveSlot == -1) 
	{
		Logger::printWarning("Texture " + textureName + " is not activated");
	}
	return textureActiveSlot;
}

int TextureManager::getTextureLocation(const std::string textureName)
{
	auto& result = textures.find(textureName);

	if (result != textures.end())
	{
		return result->second;
	}
	return -1;
}

int TextureManager::getActiveSlot(const std::string textureName)
{
	int textureLocation = getTextureLocation(textureName);
	if (textureLocation != -1)
	{
		for (int i = 0;i < 31;i++) {
			if (textureActiveSlots[i] == textureLocation)
			{
				return i;
			}
		}
	}
	return -1;
}

