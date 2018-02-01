#include "Skybox.h"
#include "TextureManager.h"
#include "Logger.h"
//#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
Skybox::Skybox()
{
	const std::string CUBE_TEXTURE_LOCATION[6] = {



		"textures/skybox/violentdays_ft.tga",
		"textures/skybox/violentdays_bk.tga",//

		"textures/skybox/violentdays_up.tga",//
		"textures/skybox/violentdays_dn.tga",//
		"textures/skybox/violentdays_rt.tga",
		"textures/skybox/violentdays_lf.tga"




	};
	float skyboxVertices[] = {         
		-100.0f,  -100.0f, -100.0f,
		-100.0f, 100.0f, -100.0f,
		100.0f, 100.0f, -100.0f,
		100.0f, -100.0f, -100.0f,

		-100.0f,  -100.0f, 100.0f,
		-100.0f, 100.0f, 100.0f,
		100.0f, 100.0f, 100.0f,
		100.0f, -100.0f, 100.0f,
	};
	unsigned short indices[] = {
		0,1,2,
		0,2,3,

		4,5,6,
		4,6,7,

		4,0,5,
		0,1,5,

		3,0,4,
		3,4,7,

		2,3,7,
		2,7,6,

		2,1,5,
		2,5,6
	};
	vao = new VertexArrayObject();
	vao->loadIndices(36,VertexArrayObject::createIndicesBuffer(36, indices));
	vao->activeAttribute(0, 3, 0);
	vao->setData(108 * sizeof(float), skyboxVertices);

	TextureManager::getTextureManager()->addCubeTexture("skybox", CUBE_TEXTURE_LOCATION);
	TextureManager::getTextureManager()->activeTexture("skybox", 0x8513);

	shader = new Shader("shaders/skybox_vertex.glsl", "shaders/skybox_fragment.glsl");
}

Skybox::~Skybox()
{
	delete vao;
	delete shader;
	TextureManager::getTextureManager()->deleteTexture("skybox");
}

void Skybox::render(const glm::mat4 projectionMatrix, const glm::mat4 rotationMatrix)
{	
	glDepthMask(GL_FALSE);
	shader->use();
	shader->setUniformValue("skybox", TextureManager::getTextureManager()->getTextureActiveSlot("skybox"));
	shader->setUniformValue("cameraSpaceMatrix", rotationMatrix);
	shader->setUniformValue("projectionMatrix", projectionMatrix);
	vao->render(4);
	glDepthMask(GL_TRUE);
}