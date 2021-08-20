/**
 BackgroundEntity
 By: Lim Chian Song
 Date: May 2020
 */
#include "BackgroundEntity.h"

//For allowing creating of Quad Mesh 
#include "Primitives/MeshBuilder.h"

//Shader manager
#include "../Library/Source/RenderControl/ShaderManager.h"

#include <iostream>
#include <vector>

using namespace std;
CBackgroundEntity::CBackgroundEntity(std::string name)
{
	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	textureName = name;

	scaleX = scaleY = 2;
}

CBackgroundEntity::~CBackgroundEntity()
{

}

bool CBackgroundEntity::Init(float scaleX, float scaleY)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	if (LoadTexture(textureName.c_str()) == false)
	{
		std::cout << "Failed to load ground tile texture" << std::endl;
		return false;
	}

	//CS: Create the Quad Mesh using the mesh builder
	mesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), 1.f, 1.f);
	this->scaleX = scaleX;
	this->scaleY = scaleY;

	return true;
}


/**
 @brief Render this instance
 */
void CBackgroundEntity::Render(void)
{
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	transform = glm::translate(transform, glm::vec3(glm::vec2(vTransform.x, vTransform.y),
		0.0f));

	transform = glm::scale(transform, glm::vec3(scaleX, scaleY, 1.f));
	// Update the shaders with the latest transform
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);

	glBindVertexArray(VAO);

	//CS: Use mesh to render
	mesh->Render();

	glBindVertexArray(0);

}