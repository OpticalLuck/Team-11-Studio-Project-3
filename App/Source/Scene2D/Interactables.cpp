#include "Interactables.h"

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

#include "Primitives/MeshBuilder.h"

#include <iostream>

Interactables::Interactables(void)
	: bInteracted(false)
	, animatedSprites(NULL)
{
	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	vTransform = glm::i32vec2(0);

	type = ENTITY_TYPE::INTERACTABLES;
}

Interactables::~Interactables(void)
{
	glDeleteVertexArrays(1, &VAO);
}

bool Interactables::Init(int iMapID)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(10, 6, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	switch (iMapID)
	{
	case BUTTON:
		// Load the interactable texture
		if (LoadTexture("Image/Cyborg/Cyborg.png", iTextureID) == false)
		{
			std::cout << "Failed to load player tile texture" << std::endl;
			return false;
		}
		break;
	default:
		break;
	}

	return true;
}

void Interactables::Update(const double dElapsedTime)
{
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
	glBindTexture(GL_TEXTURE_2D, iTextureID);

	//CS: Render the animated sprite
	animatedSprites->Render();

	glBindVertexArray(0);
}

void Interactables::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}

void Interactables::SetCommand(COMMANDS eCommand)
{
	cCommand.SetCommand(eCommand);
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