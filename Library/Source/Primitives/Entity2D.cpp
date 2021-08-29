/**
 CEntity2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Entity2D.h"
#include "MeshBuilder.h"

 // Include Shader Manager
#include "..\RenderControl\ShaderManager.h"

// Include ImageLoader
#include "..\System\ImageLoader.h"

//#include "Scene2D/Map2D.h"

#include <iostream>
using namespace std;

bool CEntity2D::recording = false;

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CEntity2D::CEntity2D(ENTITY_TYPE type)
	: VAO(0)
	, VBO(0)
	, EBO(0)
	, mesh(NULL)
	, iTextureID(0)
	, cSettings(NULL)
	, collider2D(NULL)
	, cPhysics2D(NULL)
	, camera2D(NULL)
{
	this->type = type;

	transform = glm::mat4(1.0f);	// make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	vTransform = glm::i32vec2(0);

	fRotate = 0.f;

	cSettings = CSettings::GetInstance();
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CEntity2D::~CEntity2D(void)
{
	//CS: Delete the mesh
	cSettings = nullptr;
	//cMap2D = nullptr;
	camera2D = nullptr;

 	if (mesh) {
		delete mesh;
		mesh = nullptr;
	}


	if (collider2D) {
		delete collider2D;
		collider2D = nullptr;
	}

	if (cPhysics2D) {
		delete cPhysics2D;
		cPhysics2D = nullptr;
	}
}

/**
  @brief Initialise this instance
  */
bool CEntity2D::Init(void)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();
	//Map handler
	//cMap2D = CMap2D::GetInstance();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);

	//Draw a quad for a default entity2D
	mesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), 1.0f);

	fRotate = 0.f;
	if (!collider2D)
		collider2D = new Collider2D;

	if (!cPhysics2D)
		cPhysics2D = new CPhysics2D;

	return true;
}

/**
 @brief Set the name of the shader to be used in this class instance
 @param _name The name of the Shader instance in the CShaderManager
 */
 void CEntity2D::SetShader(const std::string& _name)
 {
	 this->sShaderName = _name;
 }

/**
 @brief Update this instance
 */
void CEntity2D::Update(const double dElapsedTime)
{
}

void CEntity2D::SetRecording(bool val) {
	recording = val;
}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CEntity2D::PreRender(void)
{
	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use("2DShader");
}

/**
 @brief Render this instance
 */
void CEntity2D::Render(void)
{
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	transform = glm::translate(transform, glm::vec3(glm::vec2(vTransform.x, vTransform.y),
		0.0f));
	transform = glm::rotate(transform, glm::radians(fRotate), glm::vec3(0.f, 0.f, 1.f));
	// Update the shaders with the latest transform
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);

	glBindVertexArray(VAO);

	//CS: Use mesh to render
	mesh->Render();

	glBindVertexArray(0);

}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CEntity2D::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}

void CEntity2D::RenderCollider()
{
	collider2D->PreRender();
	collider2D->Render();
	collider2D->PostRender();
}

Collider2D* CEntity2D::GetCollider()
{
	return collider2D;
}

void CEntity2D::SetTextureID(int iTextureID)
{
	this->iTextureID = iTextureID;
}

int CEntity2D::GetTextureID() const
{
	return iTextureID;
}

CPhysics2D* CEntity2D::GetPhysics()
{
	return cPhysics2D;
}

/**
@brief Load a texture, assign it a code and store it in MapOfTextureIDs.
@param filename A const char* variable which contains the file name of the texture
*/
bool CEntity2D::LoadTexture(const char* filename)
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
	unsigned char *data = cImageLoader->Load(filename, width, height, nrChannels, true);
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

