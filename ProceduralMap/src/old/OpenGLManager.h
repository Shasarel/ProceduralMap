#pragma once
#include <ft2build.h>
#include "Shader.h"
#include "TextureManager.h"
#include "VertexArrayObject.h"
#include "TextRenderer.h"
#include "Map.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include FT_FREETYPE_H
class OpenGLManager {
private:
	TextRenderer* textRenderer;
	Map* map;

public:
	glm::mat4 projectionMatrix;
	OpenGLManager(int mapVertices, float monitorRatio);
	~OpenGLManager();
	void renderMap(glm::vec3 viewerPosition, float pitch, float yaw);
};