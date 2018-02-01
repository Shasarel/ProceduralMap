#pragma once
#include <ft2build.h>
#include <string>
#include "VertexArrayObject.h"
#include "Shader.h"
#include FT_FREETYPE_H
struct Character {
	std::string textureName;
	int sizeX, sizeY, bearingX, bearingY, advance;
};
class TextRenderer {
public:
	TextRenderer();
	~TextRenderer();

	void renderText(const std::string& text,float posX, float posY, const float sizeX, const float sizeY);
private:
	Character charTab[256];
	Shader * shader;
	VertexArrayObject* textVao;
	unsigned int indicesBufferId;
};