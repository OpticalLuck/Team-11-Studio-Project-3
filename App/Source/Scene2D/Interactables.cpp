#include "Interactables.h"

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

#include "Primitives/MeshBuilder.h"
#include "../Scene2D/EntityManager.h"

#include <iostream>

Interactables::Interactables(int iTextureID)
	: bInteraction(false)
	, quad(NULL)
	, iInteractableID(0)
{
	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	this->iTextureID = iTextureID;

	// Initialise vecIndex
	vTransform = glm::i32vec2(0);

	type = ENTITY_TYPE::INTERACTABLES;

	Init();
}

Interactables::~Interactables(void)
{
	glDeleteVertexArrays(1, &VAO);

	if (quad)
	{
		delete quad;
		quad = nullptr;
	}
}

bool Interactables::Init()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	type = CEntity2D::ENTITY_TYPE::INTERACTABLES;
	quad = CMeshBuilder::GenerateQuad();

	interactableType = static_cast<INTERACTABLE_TYPE>(iTextureID);
	if (interactableType < DOOR)
	{
		collidable = false;
	}
	else
	{
		// Initialise Doors
		// Interacted doors are open
		if (bInteraction)
		{
			collidable = false;
		}
	}

	return true;
}

void Interactables::Update(const double dElapsedTime)
{
	if (interactableType == PRESSURE_PLATE)
		bInteraction = false;
}

void Interactables::PreRender(void)
{
	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

void Interactables::Render(void)
{
	glBindVertexArray(VAO);
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	unsigned int colorLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "runtime_color");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	transform = glm::translate(transform, glm::vec3(
		CSettings::GetInstance()->ConvertIndexToUVSpace(glm::vec2(vTransform.x, vTransform.y)),
		0.0f));

	// Update the shaders with the latest transform
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform4fv(colorLoc, 1, glm::value_ptr(currentColor));

	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, CTextureManager::GetInstance()->MapOfTextureIDs.at(iTextureID));

	//CS: Render the animated sprite
	quad->Render();

	glBindVertexArray(0);
}

void Interactables::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}

void Interactables::SetInteractableID(int id)
{
	iInteractableID = id;
}

/**
@brief Load a texture, assign it a code and store it in MapOfTextureIDs.
@param filename A const char* variable which contains the file name of the texture
*/
bool Interactables::LoadTexture(const char* filename, GLuint& iTextureID)
{
	// Variables used in loading the texture
	int width, height, nrChannels;

	// texture 1
	// ---------
	glGenTextures(1, &iTextureID);
	glBindTexture(GL_TEXTURE_2D, iTextureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	CImageLoader* cImageLoader = CImageLoader::GetInstance();
	unsigned char* data = cImageLoader->Load(filename, width, height, nrChannels, true);
	if (data)
	{
		if (nrChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if (nrChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		// Generate mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		return false;
	}
	// Free up the memory of the file data read in
	free(data);

	return true;
}

bool Interactables::Activate(bool interaction)
{
	switch (interactableType) {
	case LEVER:
		ActivateSwitch();
		break;
	case PRESSURE_PLATE:
		ActivatePressurePlate();
		break;
	default:
		this->bInteraction = interaction;
		break;
	}

	// Change Texture
	// On and Off Textures are an index apart in the texture manager
	switch (bInteraction) {
	case true:
		this->iTextureID = this->interactableType + 1;
		break;
	case false:
		this->iTextureID = this->interactableType;
		break;
	}

	// Loop through the interactables to activate the corresponding Interactable IDs
	if (this->interactableType < DOOR)
	{
		CEntityManager* entManager = CEntityManager::GetInstance();
		for (auto& e : entManager->GetAllInteractables())
		{
			if (e->interactableType >= DOOR)
			{
				if (this->iInteractableID == e->iInteractableID)
				{
					e->Activate(this->bInteraction);
					e->collidable = !collidable;
					return true;
				}
			}
		}
	}

	return true;
}

bool Interactables::ActivateSwitch()
{
	this->bInteraction = !this->bInteraction;
	return false;
}

bool Interactables::ActivatePressurePlate()
{
	this->bInteraction = true;
	return false;
}
