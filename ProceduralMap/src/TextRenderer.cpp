#include "TextRenderer.h"
#include "TextureManager.h"
#include "Logger.h"

TextRenderer::TextRenderer()
{
	FT_Library ft;
	FT_Face face;
	Logger::printInfo("initializing font renderer");
	if (FT_Init_FreeType(&ft)) {
		Logger::printError("Could not init freetype library");
	}
	else {
		if (FT_New_Face(ft, "fonts/Montserrat.ttf", 0, &face)) {
			Logger::printError("Could not open font\n");
		}
		else {
			FT_Set_Pixel_Sizes(face, 0, 48);
			
			shader = new Shader("shaders/text_vertex.glsl", "shaders/text_fragment.glsl");

			unsigned short* textIndices = new unsigned short int[6];
			textIndices[0] = 0;
			textIndices[1] = 1;
			textIndices[2] = 3;
			textIndices[3] = 1;
			textIndices[4] = 2;
			textIndices[5] = 3;

			textVao = new VertexArrayObject();
			textVao->createIndices(6, textIndices);
			delete textIndices;
			textVao->activeAttribute(0, 2, 0);
			textVao->activeAttribute(1, 2, (void*)(8 * sizeof(float)));
			float box[] = {
				0,        1,
				1,        1,
				1,        0,
				0,        0,


				0,        1,
				1,        1,
				1,        0,
				0,        0
			};
			textVao->setData(sizeof(box), box);

			FT_GlyphSlot g = face->glyph;
			for (int i = 0;i < 256;i++) {
				if (FT_Load_Char(face, i, FT_LOAD_RENDER))
					continue;
				std::string textureName = "fontAscii" + std::to_string(i);
				TextureManager::getTextureManager()->addFontTexture(textureName, g->bitmap.width, g->bitmap.rows, g->bitmap.buffer);
				charTab[i].textureName = textureName;
				charTab[i].sizeX = g->bitmap.width;
				charTab[i].sizeY = g->bitmap.rows;
				charTab[i].bearingX = g->bitmap_left;
				charTab[i].bearingY = g->bitmap_top;
				charTab[i].advance = g->advance.x;
			}
			FT_Done_Face(face);
		}
		FT_Done_FreeType(ft);
	}
}

TextRenderer::~TextRenderer()
{
	delete textVao;
	delete shader;
}

void TextRenderer::renderText(const std::string & text, float posX, float posY, const float sizeX, const float sizeY)
{
	const char *p;

	for (p = text.c_str(); *p; p++) {

		float x2 = posX + charTab[*p].bearingX * sizeX;
		float y2 = posY - (charTab[*p].sizeY - charTab[*p].bearingY) * sizeY;
		float w = charTab[*p].sizeX * sizeX;
		float h = charTab[*p].sizeY * sizeY;


		shader->use();
		TextureManager::getTextureManager()->activeTexture(charTab[*p].textureName);
		shader->setUniformValue("tex", TextureManager::getTextureManager()->getTextureActiveSlot(charTab[*p].textureName));
		shader->setUniformValue("translation", glm::vec2(x2, y2));
		shader->setUniformValue("scale", glm::vec2(w, h));
		textVao->render(4);
		TextureManager::getTextureManager()->deactiveTexture(charTab[*p].textureName);
		posX += (charTab[*p].advance / 64) * sizeX;
	}
}
