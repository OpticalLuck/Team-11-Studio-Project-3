/**
 Player2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Player2D.h"

#include <iostream>
#include <sstream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"
#include "Primitives/MeshBuilder.h"

// Include Game Manager
#include "GameManager.h"

#include "Primitives/Camera2D.h"

#include "Math/MyMath.h"

//Interactables
#include "Obstacle2D.h"

//Items
#include "Projectiles.h"
#include "Bullet2D.h"

#include "EntityManager.h"
#include "Interactables.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::CPlayer2D(void)
	: cMap2D(NULL)
	, cKeyboardController(NULL)
	, cMouseController(NULL)
	, cSoundController(NULL)
	, cInputHandler(NULL)
	, iTempFrameCounter(0)
	//, bDamaged(false)
	, bIsClone(false)
	, cInventory(NULL)
	, jumpCount(0)
	, bJustTeleported(false)

{
	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	vTransform = glm::i32vec2(0);

	type = ENTITY_TYPE::PLAYER;

	animatedSprites = nullptr;
	camera = nullptr;
	cSettings = CSettings::GetInstance();

	checkpoint  = glm::i32vec2();
	currentColor = glm::vec4(1,1,1,1);

	pHealth = pShield = 0;
	pMaxShield = 0;
	pMaxHealth = 5;
	pBlinkInterval = 0;
	pMaxBlinkInterval = int(0.175f * (float)cSettings->FPS);

	for (auto &timerVal : timerArr)
	{
		timerVal.first = false;
		timerVal.second = 0;
	}
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::~CPlayer2D(void)
{
	if (collider2D) {
		delete collider2D;
		collider2D = nullptr;
	}

	// We won't delete this since it was created elsewhere
	cSoundController = NULL;

	// We won't delete this since it was created elsewhere
	// cInventoryManager = NULL;

	// We won't delete this since it was created elsewhere
	cKeyboardController = NULL;

	// We won't delete this since it was created elsewhere
	cInputHandler = NULL;

	// We won't delete this since it was created elsewhere
	cMap2D = NULL;

	CInventoryManager::GetInstance()->DeleteInventory(this->cInventory->sName);

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
}

/**
  @brief Initialise this instance
  */
bool CPlayer2D::Init(void)
{
	camera = Camera2D::GetInstance();
	cSoundController = CSoundController::GetInstance();
	cInputHandler = CInputHandler::GetInstance();
	cSettings = CSettings::GetInstance();

	cMouseController = CMouseController::GetInstance();
	cKeyboardController = CKeyboardController::GetInstance();
	cKeyboardController->Reset();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(1, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set checkpoint position to start position
	checkpoint = glm::vec2(uiCol, uiRow);
	// Set the start position of the Player to iRow and iCol
	vTransform = glm::vec2(uiCol, uiRow);
	vTransform.x;
	vTransform.y;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Load the player texture
	if (LoadTexture("Image/Cyborg/Cyborg.png", iTextureID) == false)
	{
		std::cout << "Failed to load player tile texture" << std::endl;
		return false;
	}
	
	state = STATE::S_IDLE;
	facing = FACING_DIR::RIGHT;
	//CS: Create the animated sprite and setup the animation 
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(10, 6, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("run", 0, 6);
	animatedSprites->AddAnimation("idle", 6, 10);
	animatedSprites->AddAnimation("jump", 12, 16);
	animatedSprites->AddAnimation("double_jump", 18, 24);
	animatedSprites->AddAnimation("death", 24, 30);
	animatedSprites->AddAnimation("attack", 36, 42);
	animatedSprites->AddAnimation("climb", 48, 54);
	animatedSprites->AddAnimation("hit", 54, 56);
	//CS: Play the "idle" animation as default
	animatedSprites->PlayAnimation("idle", -1, 1.0f);

	//CS: Init the color to white
	currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0); //100,100,100 for full blown white

	//Health related stuff
	pHealth = pMaxHealth;

	//Timers and stuff
	//Shield
	pMaxShield = int(2.3f * (float)cSettings->FPS);
	pShield = 0;

	//Blink Interval
	pBlinkInterval = 0;
	jumpCount = 0;

	//fMovementSpeed = 1.f;
	fMovementSpeed = 4.f;
	fJumpSpeed = 5.f;

	if (!cPhysics2D)
		cPhysics2D = new CPhysics2D;
	if (!collider2D)
		collider2D = new Collider2D;

	collider2D->Init(vTransform, glm::vec2(0.2f, 0.5f), Collider2D::ColliderType::COLLIDER_QUAD);
	// collider2D->SetOffset(glm::vec2(0.f, -0.5f));
	cPhysics2D->Init(&vTransform);

	iTempFrameCounter = 0;
	iFrameCounterEnd = 0;

	CInventoryManager::GetInstance()->Add("Player", this);
	cInventory = CInventoryManager::GetInstance()->Get("Player");

	return true;
}

bool CPlayer2D::Init(glm::i32vec2 spawnpoint, int iCloneIndex)
{
	// Store the keyboard controller singleton instance here
	cKeyboardController = CKeyboardController::GetInstance();
	// Reset all keys since we are starting a new game
	cKeyboardController->Reset();

	cMouseController = CMouseController::GetInstance();
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();

	// Erase the value of the player in the arrMapInfo
	//cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set checkpoint position to start position
	checkpoint = spawnpoint;
	// Set the start position of the Player to iRow and iCol
	vTransform = spawnpoint;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Load the player texture
	if (LoadTexture("Image/Cyborg/Cyborg.png", iTextureID) == false)
	{
		std::cout << "Failed to load player tile texture" << std::endl;
		return false;
	}

	state = STATE::S_IDLE;
	facing = FACING_DIR::RIGHT;
	//CS: Create the animated sprite and setup the animation 
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(10, 6, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("run", 0, 6);
	animatedSprites->AddAnimation("idle", 6, 10);
	animatedSprites->AddAnimation("jump", 12, 16);
	animatedSprites->AddAnimation("double_jump", 18, 24);
	animatedSprites->AddAnimation("death", 24, 30);
	animatedSprites->AddAnimation("attack", 36, 42);
	animatedSprites->AddAnimation("climb", 48, 54);
	animatedSprites->AddAnimation("hit", 54, 56);
	//CS: Play the "idle" animation as default
	animatedSprites->PlayAnimation("idle", -1, 1.0f);

	//CS: Init the color to white
	currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

	jumpCount = 0;

	camera = Camera2D::GetInstance();

	//Health related stuff
	pHealth = pMaxHealth;

	//Timers and stuff
	//Shield
	pShield = 0;
	pMaxShield = int(3.f * (float)cSettings->FPS);

	//Blink Interval
	pBlinkInterval = 0;

	jumpCount = 0;

	fMovementSpeed = 3.f;
	fJumpSpeed = 5.f;

	// Get the handler to the CSoundController
	cSoundController = CSoundController::GetInstance();

	cInputHandler = CInputHandler::GetInstance();

	if (!cPhysics2D)
		cPhysics2D = new CPhysics2D;
	if (!collider2D)
		collider2D = new Collider2D;

	collider2D->Init(vTransform, glm::vec2(0.2f, 0.5f));
	cPhysics2D->Init(&vTransform);

	std::stringstream ss;
	ss << "Clone" << iCloneIndex;

	CInventoryManager::GetInstance()->Add(ss.str().c_str(), this);
	cInventory = CInventoryManager::GetInstance()->Get(ss.str().c_str());

	return true;
}

int CPlayer2D::GetHealth(void) {
	return pHealth;
}

int CPlayer2D::GetMaxHealth(void)
{
	return pMaxHealth;
}

glm::i32vec2 CPlayer2D::GetCheckpoint(void) {
	return checkpoint;
}

/**
 @brief Reset this instance
 */
bool CPlayer2D::Reset()
{
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(1, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set checkpoint to start position
	checkpoint = glm::i32vec2(uiCol, uiRow);
	// Set the start position of the Player to iRow and iCol
	vTransform = glm::i32vec2(uiCol, uiRow);

	//CS: Reset double jump
	jumpCount = 0;

	//CS: Play the "idle" animation as default
	animatedSprites->PlayAnimation("idle", -1, 1.0f);

	//CS: Init the color to white
	currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

	return true;
}

/**
 @brief Update this instance
 */
void CPlayer2D::Update(const double dElapsedTime)
{
	DEBUG_MSG(iTempFrameCounter);

	// Only update the inputs if the instance is not a clone
	// Clone will have a fixed input that is created on initialisation
	if (!bIsClone)
	{
		m_KeyboardInputs = cInputHandler->GetAllKeyboardInputs();
		m_MouseInputs = cInputHandler->GetAllMouseInputs();
	}

	// Store the old position
	vOldTransform = vTransform;

	for (auto& timerVal : timerArr)
	{
		if (timerVal.first)
		{
			if (timerVal.second <= 60)
				timerVal.second += dElapsedTime;
			else
				timerVal.second = 0;
		}
	}

	// Get keyboard & Mouse updates
	InputUpdate(dElapsedTime);
	cPhysics2D->Update(dElapsedTime);
	// Update Collider2D Position
	collider2D->position = vTransform;

	//COLLISION RESOLUTION ON Y_AXIS AND X_AXIS
	int range = 2;
	cPhysics2D->SetboolGrounded(false);

	//Stores nearby objects and its dist to player into a vector 
	vector<pair<CObject2D*, float>> aabbVector;
	for (int row = -range; row <= range; row++) //y
	{
		for (int col = -range; col <= range; col++) //x
		{
			int rowCheck = (int)vTransform.y + row;
			int colCheck = (int)vTransform.x + col;

			if (rowCheck < 0 || colCheck < 0 || rowCheck > cMap2D->GetLevelRow() - 1 || colCheck > cMap2D->GetLevelCol() - 1) continue;
			if (cMap2D->GetCObject(colCheck, rowCheck))
			{
				CObject2D* obj = cMap2D->GetCObject(colCheck, rowCheck);
				if (obj->GetCollider()->GetbEnabled())
				{
					float distance = glm::length(obj->vTransform - vTransform);
					aabbVector.push_back({ obj, distance });
				}
			}
		}
	}
	//Sorts vector based on shortest dist from player to object
	sort(aabbVector.begin(), aabbVector.end(), [](const std::pair<CObject2D*, float>& a, const std::pair<CObject2D*, float>& b)
	{
		return a.second < b.second;
	});
	aabbVector.erase(std::unique(aabbVector.begin(), aabbVector.end()), aabbVector.end());
	// Detects and Resolves Collsion
	for (auto aabbTuple : aabbVector)
	{
		CObject2D* obj = aabbTuple.first;
		Collision data = (collider2D->CollideWith(obj->GetCollider()));
		if (std::get<0>(data))
		{
			if (obj->GetCollider()->colliderType == Collider2D::ColliderType::COLLIDER_QUAD)
			{
				collider2D->ResolveAABB(obj->GetCollider(), data);
			}
			else if (obj->GetCollider()->colliderType == Collider2D::ColliderType::COLLIDER_CIRCLE)
			{
				if (glm::dot(cPhysics2D->GetVelocity(), obj->vTransform - vTransform) > 0)
					collider2D->ResolveAABBCircle(obj->GetCollider(), data, Collider2D::ColliderType::COLLIDER_QUAD);
			}

			if (std::get<1>(data) == Direction::UP)
				cPhysics2D->SetboolGrounded(true);
			
			if (std::get<1>(data) == Direction::LEFT || std::get<1>(data) == Direction::RIGHT)
				cPhysics2D->SetVelocity(glm::vec2(0, cPhysics2D->GetVelocity().y));
			vTransform = collider2D->position;
			obj->vTransform = obj->GetCollider()->position;
		}
	}

	//BOUNDARY CHECK
	LockWithinBoundary();

	//DISABLE KNOCKED BACK STATUS
	if (cPhysics2D->GetboolKnockedBacked() && cPhysics2D->GetVelocity() == glm::vec2(0, 0))
		cPhysics2D->SetBoolKnockBacked(false);

	//animation States
	switch (state)
	{
	case STATE::S_IDLE:
		animatedSprites->PlayAnimation("idle", -1, 1.f);
		break;
	case STATE::S_MOVE:
		animatedSprites->PlayAnimation("run", -1, 0.6f);
		break;
	case STATE::S_JUMP:
		animatedSprites->PlayAnimation("jump", 1, 0.6f);
		break;
	case STATE::S_DOUBLE_JUMP:
		animatedSprites->PlayAnimation("double_jump", 1, 0.4f);
		break;
	case STATE::S_HOLD:
		animatedSprites->PlayAnimation("idle", -1, 1.2f);
		break;
	case STATE::S_ATTACK:
		animatedSprites->PlayAnimation("attack", 1, 0.6f);
		break;
	case STATE::S_CLIMB:
		animatedSprites->PlayAnimation("climb", 1, 1.f);
		break;
	case STATE::S_DEATH:
		animatedSprites->PlayAnimation("death", 1, 3.f);
		break;
	}

	//Health
	UpdateHealthLives();

	//CS: Update the animated sprite
	animatedSprites->Update(dElapsedTime);

	if (!bIsClone || iTempFrameCounter < m_KeyboardInputs.size() - 1)
		iTempFrameCounter++;
}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CPlayer2D::PreRender(void)
{
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
void CPlayer2D::Render(void)
{
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
	actualPos = cSettings->ConvertIndexToUVSpace(actualPos) * Camera2D::GetInstance()->getZoom();

	transform = glm::translate(transform, glm::vec3(actualPos.x, actualPos.y, 0.f));
	transform = glm::scale(transform, glm::vec3(Camera2D::GetInstance()->getZoom()));

	if (facing == FACING_DIR::LEFT)
		transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
	// Update the shaders with the latest transform
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform4fv(colorLoc, 1, glm::value_ptr(currentColor));

	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);

	//CS: Render the animated sprite
	animatedSprites->Render();

	glBindVertexArray(0);

}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CPlayer2D::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}

CPlayer2D::STATE CPlayer2D::Getstate() const
{
	return state;
}

/**
@brief Load a texture, assign it a code and store it in MapOfTextureIDs.
@param filename A const char* variable which contains the file name of the texture
*/
bool CPlayer2D::LoadTexture(const char* filename, GLuint& iTextureID)
{
	// Variables used in loading the texture
	int width, height, nrChannels;
	
	// texture 1
	// ---------
	glGenTextures(1, &iTextureID);
	glBindTexture(GL_TEXTURE_2D, iTextureID);
	// set the texture wrapping parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
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

void CPlayer2D::InputUpdate(double dt)
{
	state = STATE::S_IDLE;
	
	if ((unsigned)iTempFrameCounter >= m_KeyboardInputs.size())
		return;

	glm::vec2 velocity = cPhysics2D->GetVelocity();
	glm::vec2 force = glm::vec2(0.f);

	if (m_KeyboardInputs[iTempFrameCounter][KEYBOARD_INPUTS::W].bKeyDown)
	{
		velocity.y = fMovementSpeed;
		cPhysics2D->SetboolGrounded(false);
		//DEBUG_MSG(this << ": Frame:" << iTempFrameCounter << " Move Up");
	}
	else if (m_KeyboardInputs[iTempFrameCounter][KEYBOARD_INPUTS::S].bKeyDown)
	{
		//velocity.y = -fMovementSpeed;
		//DEBUG_MSG(this << ": Frame:" << iTempFrameCounter << " Move Down");
	}

	if (m_KeyboardInputs[iTempFrameCounter][KEYBOARD_INPUTS::D].bKeyDown)
	{
		//velocity.x = fMovementSpeed;
		if (velocity.x < fMovementSpeed && !cPhysics2D->GetboolKnockedBacked())
			velocity.x = Math::Min(velocity.x + fMovementSpeed, fMovementSpeed);

		state = STATE::S_MOVE;
		facing = FACING_DIR::RIGHT;
		//DEBUG_MSG(this << ": Frame:" << iTempFrameCounter << " Move Right");
	}
	else if (m_KeyboardInputs[iTempFrameCounter][KEYBOARD_INPUTS::A].bKeyDown)
	{
		if (velocity.x > -fMovementSpeed && !cPhysics2D->GetboolKnockedBacked())
			velocity.x = Math::Max(velocity.x - fMovementSpeed, -fMovementSpeed);
		state = STATE::S_MOVE;
		facing = FACING_DIR::LEFT;
		//DEBUG_MSG(this << ": Frame:" << iTempFrameCounter << " Move Left");
	}

	if (m_KeyboardInputs[iTempFrameCounter][KEYBOARD_INPUTS::SPACE].bKeyDown)
	{
		if (jumpCount < 2 &&
			m_KeyboardInputs[iTempFrameCounter][KEYBOARD_INPUTS::SPACE].bKeyPressed)
		{
			cPhysics2D->SetboolGrounded(false);
			velocity.y = 6.5f * (jumpCount + (1 - jumpCount * 0.6f));
			jumpCount++;
			timerArr[A_JUMP].second = 0.1;
		}

		if (timerArr[A_JUMP].second < 0.2)
		{
			timerArr[A_JUMP].first = true;
			force.y = 20;

			cPhysics2D->SetboolGrounded(false);
			cPhysics2D->SetBoolKnockBacked(false);
		}
		else
		{
			timerArr[A_JUMP].first = false;

			if (cPhysics2D->GetboolGrounded())
			{
				force.y = 20;
			}
			cPhysics2D->SetboolGrounded(false);
		}
	}
	else
	{
		timerArr[A_JUMP].first = false;
		if (cPhysics2D->GetboolGrounded())
		{
			timerArr[A_JUMP].second = 0;
			jumpCount = 0;
		}
	}

	if (glm::length(velocity) > 0.f)
		cPhysics2D->SetVelocity(velocity);

	if (glm::length(force) > 0.f)
		cPhysics2D->SetForce(force);

	if (m_MouseInputs[iTempFrameCounter][MOUSE_INPUTS::LMB].bButtonPressed)
	{
		cSoundController->PlaySoundByID(SOUND_ID::SOUND_SWING);
		cInventory->UseItem();
	}

	cInventory->Update(dt, iTempFrameCounter ,m_KeyboardInputs, m_MouseInputs);
}
void CPlayer2D::SetClone(bool bIsClone)
{
	this->bIsClone = bIsClone;
}

bool CPlayer2D::IsClone()
{
	return bIsClone;
}

void CPlayer2D::Attacked(int hp, CPhysics2D* bounceObj) {
	if (pShield > 0) //Return if shield is enabled/ player does not get damaged
		return;

	pHealth = Math::Max(0, pHealth - 1);
	pShield = pMaxShield + 1; //Offset by 1 frame for better synchronisation (FUTURE JEVON IF YOU KNOW YOU KNOW IF NOT THEN LMAO)

	//Collision response between the objects
	if (bounceObj) {
		glm::vec2 ogVel = cPhysics2D->GetVelocity();

		if (vTransform.x > bounceObj->GetPosition().x)
			cPhysics2D->SetVelocity(glm::vec2(-fMovementSpeed, ogVel.y));
		else if (vTransform.x < bounceObj->GetPosition().x)
			cPhysics2D->SetVelocity(glm::vec2(fMovementSpeed, ogVel.y));

		cPhysics2D->CollisionResponse(bounceObj,1.5f,1.5f);
		cPhysics2D->SetBoolKnockBacked(true);
		bounceObj->SetBoolKnockBacked(true);
	}
}

float CPlayer2D::GetTransformX(void)
{
	return vTransform.x;
}

float CPlayer2D::GetTransformY(void)
{
	return vTransform.y;
}

CInventory* CPlayer2D::GetInventory(void)
{
	return cInventory;
}

void CPlayer2D::SetKeyInputs(std::vector<std::array<KeyInput, KEYBOARD_INPUTS::KEY_TOTAL>> inputs)
{
	m_KeyboardInputs = inputs;
}

void CPlayer2D::SetMouseInputs(std::vector<std::array<MouseInput, MOUSE_INPUTS::MOUSE_TOTAL>> inputs)
{
	m_MouseInputs = inputs;
}

void CPlayer2D::ResetToCheckPoint()
{
	vTransform = checkpoint;
	cPhysics2D->SetVelocity(glm::vec2(0.f));
	cKeyboardController->Reset();
}

void CPlayer2D::LockWithinBoundary()
{
	glm::vec2 minVal = glm::vec2(0.5f, 0.f) - glm::vec2(collider2D->vec2Dimensions.x, 0);
	minVal *= -1;

	glm::vec2 mapDimensions = cMap2D->GetLevelLimit();
	glm::vec2 maxVel = mapDimensions - glm::vec2(1.f, 1.f) + glm::vec2(0.5f - collider2D->vec2Dimensions.x, 0);

	glm::vec2 oldTransform = vTransform;

	vTransform = glm::clamp(vTransform, minVal, maxVel);
	collider2D->SetPosition(vTransform);

	if (oldTransform != vTransform)
		cPhysics2D->SetVelocity(glm::vec2(0, 0));
}

CPlayer2D* const CPlayer2D::Clone()
{
	return new CPlayer2D(*this);
}

CPhysics2D* CPlayer2D::GetCPhysics(void) {
	return cPhysics2D;
}
