#include "Skybox.h"
#include "TextureManager.h"
#include "Logger.h"
//#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
Skybox::Skybox()
{
	const std::string CUBE_TEXTURE_LOCATION[6] = {



		"textures/skybox/cloudtop_ft.tga",
		"textures/skybox/cloudtop_bk.tga",//

		"textures/skybox/cloudtop_up.tga",//
		"textures/skybox/cloudtop_dn.tga",//
		"textures/skybox/cloudtop_rt.tga",
		"textures/skybox/cloudtop_lf.tga"




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

		5,4,6,
		6,4,7,

		0,4,5,
		1,0,5,

		0,3,4,
		4,3,7,

		3,2,7,
		7,2,6,

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
