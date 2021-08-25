/**
 CEnemy2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Enemy2D.h"

#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"
// Include Mesh Builder
#include "Primitives/MeshBuilder.h"

#include "EntityManager.h"

//Include entity manager
#include "EntityManager.h"

// Include GLEW
#include <GL/glew.h>

// Include ImageLoader
#include "System\ImageLoader.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"
// Include math.h
#include <math.h>

#include "../Library/Source/System/Debug.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CEnemy2D::CEnemy2D(void)
	: bIsActive(false)
	, cMap2D(NULL)
	, quadMesh(nullptr)
	, sCurrentFSM(FSM::IDLE)
	,dir(DIRECTION::LEFT)
{
	cSettings = CSettings::GetInstance();

	transform = glm::mat4(1.0f);	// make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	vTransform = glm::i32vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);

	//type = ENEMY;
	int chance = Math::RandIntMinMax(0, 100);

	//Store initial value of each round
	for (int i = 0; i < 5; i++) {
		roundDir[i] = RandomiseDir();
	}
	dir = roundDir[0];
	pHealth = 1;

	pShield = pBlinkInterval = 0;

	pMaxShield = int(2.3f * (float)cSettings->FPS);
	pMaxBlinkInterval = int(0.175f * (float)cSettings->FPS);

	currFrame = 0;
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CEnemy2D::~CEnemy2D(void)
{
	if (quadMesh) {
		delete quadMesh;
		quadMesh = nullptr;
	}

	cMap2D = nullptr;
	camera = nullptr;
	cSettings = nullptr;
	cEntityManager = nullptr;

	currTarget = nullptr;
	for (unsigned i = 0; i < arrPlayer.size(); i++)
		arrPlayer[i] = nullptr;
	arrPlayer.clear();
	
	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

/**
  @brief Initialise this instance
  */
bool CEnemy2D::Init(void)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();

	camera = Camera2D::GetInstance();

	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(300, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	vTransform = glm::i32vec2(uiCol, uiRow);
	
	dir = DIRECTION::LEFT;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS: Create the Quad Mesh using the mesh builder
	quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	roundIndex = 0;

	// Load the enemy2D texture
	if (LoadTexture("Image/Scene2D_EnemyTile.tga", iTextureID) == false)
	{
		std::cout << "Failed to load enemy2D tile texture" << std::endl;
		return false;
	}

	//Store initial value of each round
	for (int i = 0; i < 5; i++) {
		roundDir[i] = RandomiseDir();
	}

	//CS: Init the color to white
	currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

	//Get handlers to all player objects
	currTarget = nullptr;
	arrPlayer = CEntityManager::GetInstance()->GetAllPlayers();

	cEntityManager = CEntityManager::GetInstance();

	if (!cPhysics2D)
		cPhysics2D = new CPhysics2D;
	if (!collider2D)
		collider2D = new Collider2D;
	// Set the Physics to fall status by default
	//cPhysics2D.Init();
	//cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;
	pHealth = 1;

	return true;
}

CEnemy2D::FSM CEnemy2D::RandomiseFSM(void) {
	int rand = Math::RandIntMinMax((int)FSM::IDLE, (int)FSM::PATROL);
	return FSM(rand);
}

void CEnemy2D::SetTexture(const char* fileName) {
	if (!LoadTexture(fileName, iTextureID)) {
		DEBUG_MSG("ERROR: FAILED TO SET TEXTURE ON ENEMY. INVALID FILENAME.");
	}
}

CPlayer2D* CEnemy2D::GetNearestTarget(float dist) {
	float indexDist = dist; //Approx distance the target has to be within to be counted on
	float minDist = indexDist;

	CPlayer2D* nearest = nullptr;

	for (unsigned i = 0; i < arrPlayer.size(); i++) {
		CPlayer2D* currPlayer = arrPlayer[i];

		float currDist = glm::length((currPlayer->vTransform - vTransform));

		if (currDist <= indexDist && currDist <= minDist) {
			minDist = currDist;
			nearest = currPlayer;
		}
	}

	if (!WithinProjectedCamera(nearest))
		nearest = nullptr;

	return nearest;
}

CEnemy2D::DIRECTION CEnemy2D::RandomiseDir(void) {
	return (DIRECTION)Math::RandIntMinMax(0, (int)DIRECTION::NUM_DIRECTIONS - 1);
}

float CEnemy2D::GetAngle(glm::vec2 pos) {
	glm::vec2 dirVec = glm::normalize(pos - vTransform);

	float angle = atan2f(dirVec.y, dirVec.x);
	angle = Math::RadianToDegree(angle);

	if (angle < 0)
		angle += 360;
	else if (angle >= 360)
		angle -= 360;

	return angle;
}

void CEnemy2D::Attacked(int hp) {
	if (pShield > 0) //Return if shield is enabled/ enemy does not get damaged (NOTICE: KEEP FOR NOW BUT REMOVE LATER)
		return;

	pHealth = Math::Max(0, pHealth - 1);
	pShield = pMaxShield + 1; //Offset by 1 frame for better synchronisation (FUTURE JEVON IF YOU KNOW YOU KNOW IF NOT THEN LMAO)
}

float CEnemy2D::GetDistanceBetweenPlayer(CPlayer2D* player) {
	return glm::length(player->vTransform - vTransform);
}

bool CEnemy2D::WithinProjectedCamera(CPlayer2D* player) {
	//Get camera transforms and use them instead
	glm::vec2 offset = glm::vec2((cSettings->NUM_TILES_XAXIS / 2.f) - 0.5f, (cSettings->NUM_TILES_YAXIS / 2.f) - 0.5f);

	//Camera init
	glm::vec2 cameraPos = player->vTransform;
	//Clamping of camera
	float xOffset = ((float)cSettings->NUM_TILES_XAXIS / 2.f) - 1;
	float yOffset = ((float)cSettings->NUM_TILES_YAXIS / 2.f) - 1;

	glm::vec2 clampPos = cMap2D->GetLevelLimit();

	//Clamping of X axis
	if (cameraPos.x < xOffset)
		cameraPos.x = xOffset;
	else if (cameraPos.x > clampPos.x - xOffset - 2)
		cameraPos.x = clampPos.x - xOffset - 2;

	//Clamping of Y axis
	if (cameraPos.y < yOffset)
		cameraPos.y = yOffset;
	else if (cameraPos.y > clampPos.y - yOffset - 2)
		cameraPos.y = clampPos.y - yOffset - 2;

	glm::vec2 IndexPos = vTransform;

	glm::vec2 actualPos = IndexPos - cameraPos + offset;
	actualPos = cSettings->ConvertIndexToUVSpace(actualPos);

	float clampOffset = cSettings->ConvertIndexToUVSpace(CSettings::AXIS::x, 1, false);
	clampOffset = (clampOffset + 1);

	float clampX = 1.0f + clampOffset;
	float clampY = 1.0f + clampOffset;
	if (actualPos.x <= -clampX || actualPos.x >= clampX || actualPos.y <= -clampY || actualPos.y >= clampY)
		return false; //Return false if enemy is too far from projected camera
	else
		return true;
}

/**
 @brief Update this instance
 */
void CEnemy2D::Update(const double dElapsedTime)
{
	
}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CEnemy2D::PreRender(void)
{
	if (!bIsActive)
		return;

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render this instance
 */
void CEnemy2D::Render(void)
{
	if (!bIsActive)
		return;

	glBindVertexArray(VAO);
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	unsigned int colorLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "runtime_color");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	//Get camera transforms and use them instead
	glm::vec2 offset = glm::i32vec2((cSettings->NUM_TILES_XAXIS / 2), (cSettings->NUM_TILES_YAXIS / 2));
	glm::vec2 cameraPos = camera->getCurrPos();

	glm::vec2 IndexPos = vTransform;

	glm::vec2 actualPos = IndexPos - cameraPos + offset;
	actualPos = cSettings->ConvertIndexToUVSpace(actualPos);

	float clampOffset = cSettings->ConvertIndexToUVSpace(CSettings::AXIS::x, 1, false);
	clampOffset = (clampOffset + 1);

	float clampX = 1.0f + clampOffset;
	float clampY = 1.0f + clampOffset;
	if (actualPos.x <= -clampX || actualPos.x >= clampX || actualPos.y <= -clampY || actualPos.y >= clampY)
		return; //Exit code if enemy is too far to be rendered

	transform = glm::translate(transform, glm::vec3(actualPos.x, actualPos.y, 0.f));

	// Update the shaders with the latest transform
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform4fv(colorLoc, 1, glm::value_ptr(currentColor));

	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);

	// Render the tile
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	quadMesh->Render();

	glBindVertexArray(0);

}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CEnemy2D::PostRender(void)
{
	if (!bIsActive)
		return;

	// Disable blending
	glDisable(GL_BLEND);
}

/**
@brief Set the indices of the enemy2D
@param iIndex_XAxis A const int variable which stores the index in the x-axis
@param iIndex_YAxis A const int variable which stores the index in the y-axis
*/
void CEnemy2D::SetTransform(const int iIndex_XAxis, const int iIndex_YAxis)
{
	this->vTransform.x = (float)iIndex_XAxis;
	this->vTransform.y = (float)iIndex_YAxis;
}


/**
@brief Load a texture, assign it a code and store it in MapOfTextureIDs.
@param filename A const char* variable which contains the file name of the texture
*/
bool CEnemy2D::LoadTexture(const char* filename, GLuint& iTextureID)
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

