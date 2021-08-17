/**
 Player2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Player2D.h"

#include <iostream>
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

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::CPlayer2D(void)
	: cMap2D(NULL)
	, cKeyboardController(NULL)
	// , cInventoryManager(NULL)
	, cInventoryItem(NULL)
	, cSoundController(NULL)
	, cKeyboardInputHandler(NULL)
	, iTempFrameCounter(0)
	//, bDamaged(false)
	, bIsClone(false)
{
	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	vTransform = glm::i32vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::~CPlayer2D(void)
{
	// We won't delete this since it was created elsewhere
	cSoundController = NULL;

	// We won't delete this since it was created elsewhere
	// cInventoryManager = NULL;

	// We won't delete this since it was created elsewhere
	cKeyboardController = NULL;

	// We won't delete this since it was created elsewhere
	cKeyboardInputHandler = NULL;

	// We won't delete this since it was created elsewhere
	cMap2D = NULL;

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
}

/**
  @brief Initialise this instance
  */
bool CPlayer2D::Init(void)
{
	// Store the keyboard controller singleton instance here
	cKeyboardController = CKeyboardController::GetInstance();
	// Reset all keys since we are starting a new game
	cKeyboardController->Reset();

	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(1, true, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	 //cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set checkpoint position to start position
	checkpoint = glm::i32vec2(uiCol, uiRow);
	// Set the start position of the Player to iRow and iCol
	vTransform = glm::i32vec2(uiCol, uiRow);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Load the player texture
	if (LoadTexture("Image/Cyborg/Cyborg.png", iTextureID) == false)
	{
		std::cout << "Failed to load player tile texture" << std::endl;
		return false;
	}
	
	state = S_IDLE;
	facing = RIGHT;
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

	// Get the handler to the CInventoryManager instance
    // cInventoryManager = CInventoryManager::GetInstance();
	// Add a Lives icon as one of the inventory items
	// cInventoryItem = cInventoryManager->Add("Lives", "Image/Collectibles/Scene2D_Lives.tga", 5, 3);
	// cInventoryItem->vec2Size = glm::vec2(25, 25);

	// Add a Health icon as one of the inventory items
	// cInventoryItem = cInventoryManager->Add("Health", "Image/Scene2D_Health.tga", 100, 100);
	// cInventoryItem->vec2Size = glm::vec2(25, 25);

	jumpCount = 0;

	//fMovementSpeed = 20.f;
	//fJumpSpeed = 400.f;

	fMovementSpeed = 5.f;
	fJumpSpeed = 5.f;

	// Get the handler to the CSoundController
	cSoundController = CSoundController::GetInstance();

	cKeyboardInputHandler = CKeyboardInputHandler::GetInstance();

	collider2D->Init();
	cPhysics2D.Init(&vTransform);

	return true;
}

/**
 @brief Reset this instance
 */
bool CPlayer2D::Reset()
{
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(1, true, uiRow, uiCol) == false)
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
	// Store the old position
	vOldTransform = vTransform;

	if (state != S_DEATH)
	{
		if (state != S_ATTACK)
		{
			// Get keyboard updates
			MovementUpdate(dElapsedTime);
		}
		if (cKeyboardController->IsKeyPressed(GLFW_KEY_V))
		{
			if (state != S_ATTACK)
			{
				cSoundController->PlaySoundByID(6);
				state = S_ATTACK;
			}
		}
	}

	cPhysics2D.Update(dElapsedTime);

	// Update Collider2D Position
	collider2D->position = glm::vec3(vTransform, 0.f);

	for (unsigned uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
	{
		for (unsigned uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; uiCol++)
		{
			if (cMap2D->GetCollider(uiRow, uiCol))
			{
				if (collider2D->CollideWith(cMap2D->GetCollider(uiRow, uiCol)))
				{
					//Collider2D::CorrectedAxis axis = collider2D.ResolveCollision(cMap2D->GetCollider(uiRow, uiCol));
					collider2D->ResolveCollisionX(cMap2D->GetCollider(uiRow, uiCol));
					// Resolve transform to corrected position in collider
					vTransform = collider2D->position;
				}
			}
		}
	}

	for (unsigned uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
	{
		for (unsigned uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; uiCol++)
		{
			if (cMap2D->GetCollider(uiRow, uiCol))
			{
				if (collider2D->CollideWith(cMap2D->GetCollider(uiRow, uiCol)))
				{
					cPhysics2D.SetVelocity(glm::vec2(cPhysics2D.GetVelocity().x, 0));
 					collider2D->ResolveCollisionY(cMap2D->GetCollider(uiRow, uiCol));
					// Resolve transform to corrected position in collider
					vTransform = collider2D->position;
				}
			}
		}
	}

	
	//animation States
	switch (state)
	{
	case S_IDLE:
		animatedSprites->PlayAnimation("idle", -1, 1.f);
		break;
	case S_MOVE:
		animatedSprites->PlayAnimation("run", -1, 0.6f);
		break;
	case S_JUMP:
		animatedSprites->PlayAnimation("jump", 1, 0.6f);
		break;
	case S_DOUBLE_JUMP:
		animatedSprites->PlayAnimation("double_jump", 1, 0.4f);
		break;
	case S_HOLD:
		animatedSprites->PlayAnimation("idle", -1, 1.2f);
		break;
	case S_ATTACK:
		animatedSprites->PlayAnimation("attack", 1, 0.6f);
		break;
	case S_CLIMB:
		animatedSprites->PlayAnimation("climb", 1, 1.f);
		break;
	case S_DEATH:
		animatedSprites->PlayAnimation("death", 1, 3.f);
		break;
	}

	//CS: Update the animated sprite
	animatedSprites->Update(dElapsedTime);

	// Update the UV Coordinates
	vec2UVCoordinate = cSettings->ConvertIndexToUVSpace(vTransform);

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
	transform = glm::translate(transform, glm::vec3(vec2UVCoordinate.x,
													vec2UVCoordinate.y,
													0.0f));
	if (facing == LEFT)
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

void CPlayer2D::MovementUpdate(double dt)
{
	state = S_IDLE;
	
	std::vector<std::array<bool, KEYBOARD_INPUTS::INPUT_TOTAL>> keyboardInputs = (bIsClone) ? m_CloneKeyboardInputs : cKeyboardInputHandler->GetAllInputs();
	if (iTempFrameCounter >= keyboardInputs.size())
		return;

	glm::vec2 velocity = cPhysics2D.GetVelocity();
	if (keyboardInputs[iTempFrameCounter][KEYBOARD_INPUTS::W])
	{
		velocity.y = fMovementSpeed;
	}
	else if (keyboardInputs[iTempFrameCounter][KEYBOARD_INPUTS::S])
	{
		velocity.y = -fMovementSpeed;
	}
	if (keyboardInputs[iTempFrameCounter][KEYBOARD_INPUTS::D])
	{
		velocity.x = fMovementSpeed;
		state = S_MOVE;
		facing = RIGHT;
	}
	else if (keyboardInputs[iTempFrameCounter][KEYBOARD_INPUTS::A])
	{
		velocity.x = -fMovementSpeed;
		state = S_MOVE;
		facing = LEFT;
	}

	if (keyboardInputs[iTempFrameCounter][KEYBOARD_INPUTS::SPACE])
	{
		velocity.y = fJumpSpeed;
	}

	if (glm::length(velocity) > 0.f)
		cPhysics2D.SetVelocity(velocity);
}

/**
 @brief Update the health and lives.
 */
void CPlayer2D::UpdateHealthLives(void)
{
	// Update health and lives
	// cInventoryItem = cInventoryManager->GetItem("Health");
	// Check if a life is lost
	if (cInventoryItem->GetCount() <= 0)
	{
		state = S_DEATH;
		// Reset the Health to max value
		// cInventoryItem->iItemCount = cInventoryItem->GetMaxCount();
		// But we reduce the lives by 1.
	// 	cInventoryItem = cInventoryManager->GetItem("Lives");
	// 	cInventoryItem->Remove(1);
		cSoundController->PlaySoundByID(9);

		// Check if there is no lives left...
		if (cInventoryItem->GetCount() < 0)
		{
			// Player loses the game
			CGameManager::GetInstance()->bPlayerLost = true;
		}
	}
}
//
//void CPlayer2D::UpdateKeyboardInputs(void)
//{
//	std::array<bool, INPUT_TOTAL> currentFrameInputs;
//	for (int i = 0; i < INPUT_TOTAL; ++i)
//	{
//		currentFrameInputs[i] = false;
//	}
//
//	if (cKeyboardController->IsKeyDown(GLFW_KEY_W))
//		currentFrameInputs[W] = true;
//
//	if (cKeyboardController->IsKeyDown(GLFW_KEY_A))
//		currentFrameInputs[A] = true;
//
//	if (cKeyboardController->IsKeyDown(GLFW_KEY_S))
//		currentFrameInputs[S] = true;
//
//	if (cKeyboardController->IsKeyDown(GLFW_KEY_D))
//		currentFrameInputs[D] = true;
//
//	if (cKeyboardController->IsKeyDown(GLFW_KEY_SPACE))
//		currentFrameInputs[SPACE] = true;
//
//	mKeyboardInputs.push_back(currentFrameInputs);
//}

void CPlayer2D::Hit(int health)
{
	if (state != S_DEATH)
	{
		// cInventoryItem = cInventoryManager->GetItem("Health");
		// cInventoryItem->Remove(health);
	}
}
void CPlayer2D::SetClone(bool bIsClone)
{
	this->bIsClone = bIsClone;
}
bool CPlayer2D::IsClone()
{
	return bIsClone;
}
void CPlayer2D::SetInputs(std::vector<std::array<bool, KEYBOARD_INPUTS::INPUT_TOTAL>> inputs)
{
	m_CloneKeyboardInputs = inputs;
}
