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

#include "Camera2D.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::CPlayer2D(void)
	: cMap2D(NULL)
	, cKeyboardController(NULL)
	, cInventoryManager(NULL)
	, cInventoryItem(NULL)
	, cSoundController(NULL)
	, runtimer(0)
	, jumptimer(0)
	, deathtimer(0)
	, attacktimer(0)
	, invulTimer(0)
	, bDamaged(false)
{
	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	i32vec2Index = glm::i32vec2(0);

	// Initialise vecNumMicroSteps
	i32vec2NumMicroSteps = glm::i32vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);

	animatedSprites = nullptr;
	camera = nullptr;
	checkpoint = i32vec2OldIndex = glm::i32vec2();
	currentColor = glm::vec4();
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::~CPlayer2D(void)
{
	// We won't delete this since it was created elsewhere
	cSoundController = NULL;

	// We won't delete this since it was created elsewhere
	cInventoryManager = NULL;

	// We won't delete this since it was created elsewhere
	cKeyboardController = NULL;

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
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set checkpoint position to start position
	checkpoint = glm::i32vec2(uiCol, uiRow);
	// Set the start position of the Player to iRow and iCol
	i32vec2Index = glm::i32vec2(uiCol, uiRow);
	// By default, microsteps should be zero
	i32vec2NumMicroSteps = glm::i32vec2(0, 0);

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

	// Set the Physics to fall status by default
	cPhysics2D.Init();
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

	// Get the handler to the CInventoryManager instance
	cInventoryManager = CInventoryManager::GetInstance();
	// Add a Lives icon as one of the inventory items
	cInventoryItem = cInventoryManager->Add("Lives", "Image/Collectibles/Scene2D_Lives.tga", 5, 3);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	// Add a Health icon as one of the inventory items
	cInventoryItem = cInventoryManager->Add("Health", "Image/Scene2D_Health.tga", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	jumpCount = 0;

	camera = Camera2D::GetInstance();

	// Get the handler to the CSoundController
	cSoundController = CSoundController::GetInstance();

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
	i32vec2Index = glm::i32vec2(uiCol, uiRow);
	// By default, microsteps should be zero
	i32vec2NumMicroSteps = glm::i32vec2(0, 0);

	//Set it to fall upon entering new level
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

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
	runtimer += dElapsedTime;
	jumptimer += dElapsedTime;
	// Store the old position
	i32vec2OldIndex = i32vec2Index;
	if (bDamaged)
	{
		if (invulTimer > 1.f || state == S_DEATH)
		{
			invulTimer = 0;
			bDamaged = false;
			currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
		}
		else
		{
			invulTimer += dElapsedTime;
			currentColor = glm::vec4(1.f, 0.f, 0.f, 0.8f);
		}
	}

	if (state != S_DEATH)
	{
		if (state != S_ATTACK)
		{
			if (runtimer > 0.01)
			{
				runtimer = 0;
				// Get keyboard updates
				if (cKeyboardController->IsKeyDown(GLFW_KEY_A))
				{
					cPhysics2D.SetInitialVelocity(glm::vec2(0.f, cPhysics2D.GetInitialVelocity().y));
					cPhysics2D.Update();
					// Calculate the new position to the left
					if (i32vec2Index.x >= 0)
					{
						i32vec2NumMicroSteps.x--;
						if (i32vec2NumMicroSteps.x < 0)
						{
							i32vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
							i32vec2Index.x--;
						}
					}

					// Constraint the player's position within the screen boundary
					Constraint(LEFT);

					// If the new position is not feasible, then revert to old position
					if (CheckPosition(LEFT) == false)
					{
						i32vec2Index = i32vec2OldIndex;
						i32vec2NumMicroSteps.x = 0;
						if (IsMidAir() == true)
						{
							jumpCount = 0;
							cPhysics2D.SetStatus(CPhysics2D::STATUS::HOLD);
						}
					}

					// Check if player is in mid-air, such as walking off a platform
					if (IsMidAir() == true)
					{
						if (cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP && (cPhysics2D.GetStatus() != CPhysics2D::STATUS::HOLD || facing == RIGHT))
							cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
					}

					facing = LEFT;
					if(state != S_JUMP && state != S_DOUBLE_JUMP)
						state = S_MOVE;
				}
				else if (cKeyboardController->IsKeyDown(GLFW_KEY_D))
				{

					cPhysics2D.SetInitialVelocity(glm::vec2(0.f, cPhysics2D.GetInitialVelocity().y));
					cPhysics2D.Update();

					// Calculate the new position to the right
					if (i32vec2Index.x < cMap2D->GetLevelCol())
					{
						i32vec2NumMicroSteps.x++;

						if (i32vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
						{
							i32vec2NumMicroSteps.x = 0;
							i32vec2Index.x++;
						}
					}

					// Constraint the player's position within the screen boundary
					Constraint(RIGHT);

					// If the new position is not feasible, then revert to old position
					if (CheckPosition(RIGHT) == false)
					{
						i32vec2NumMicroSteps.x = 0;
						if (IsMidAir() == true)
						{
							jumpCount = 0;
							cPhysics2D.SetStatus(CPhysics2D::STATUS::HOLD);
						}
					}

					// Check if player is in mid-air, such as walking off a platform
					if (IsMidAir() == true)
					{
						if (cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP && (cPhysics2D.GetStatus() != CPhysics2D::STATUS::HOLD || facing == LEFT))
						{
							cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
						}
					}
					facing = RIGHT;
					if (state != S_JUMP && state != S_DOUBLE_JUMP)
						state = S_MOVE;
				}
				else
				{
					if (state == S_MOVE && state != S_DEATH)
						state = S_IDLE;

					if (IsMidAir() && cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP && cPhysics2D.GetStatus() != CPhysics2D::STATUS::HOLD)
					{
						cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
					}
				}
			}
			if (cKeyboardController->IsKeyPressed(GLFW_KEY_SPACE))
			{
				if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::IDLE)
				{
					cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
					cPhysics2D.SetInitialVelocity(glm::vec2(0.0f, 2.f));
					jumpCount += 1;
					// Play a jump sound
					cSoundController->PlaySoundByID(3);
					state = S_JUMP;
				}
				else if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::HOLD)
				{
					cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
					if (facing == LEFT)
					{
						cPhysics2D.SetInitialVelocity(glm::vec2(1.0f, 1.5f));
						facing = RIGHT;
					}
					else if (facing == RIGHT)
					{
						cPhysics2D.SetInitialVelocity(glm::vec2(-1.0f, 1.5f));
						facing = LEFT;
					}
					jumpCount += 1;
					cSoundController->PlaySoundByID(3);
					state = S_JUMP;
				}
				else
				{
					if (jumpCount < 2)
					{
						float velX = cPhysics2D.GetInitialVelocity().x;
						cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
						cPhysics2D.SetDisplacement(glm::vec2(0.0f, 0.f));
						cPhysics2D.SetInitialVelocity(glm::vec2(velX, 2.f));
						cPhysics2D.SetTime(0);
						jumpCount += 1;
						// Play a jump sound
						cSoundController->PlaySoundByID(3);
						state = S_DOUBLE_JUMP;
					}
				}
			}
		}
		else
		{
			if (attacktimer > 0.4)
			{
				attacktimer = 0;
				state = S_IDLE;
			}
		}
		if (cKeyboardController->IsKeyPressed(GLFW_KEY_V))
		{
			if (state != S_ATTACK)
			{
				cSoundController->PlaySoundByID(6);
				state = S_ATTACK;
			}
		}

		// Update Jump or Fall
		//CS: Will cause error when debugging. Set to default elapsed time
		if (jumptimer > 0.01)
		{
			jumptimer = 0;
			UpdateJumpFall(dElapsedTime);
		}
		// Interact with the Map
		InteractWithMap();

		// Update the Health and Lives
		UpdateHealthLives();
	}
	else
	{
		if (deathtimer >= 3.f)
		{
			deathtimer = 0;
			state = S_IDLE;
			i32vec2Index = checkpoint;
			currentColor = glm::vec4(1, 1, 1, 1);
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		}
	}

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
		attacktimer += dElapsedTime;
		break;
	case S_CLIMB:
		animatedSprites->PlayAnimation("climb", 1, 1.f);
		break;
	case S_DEATH:
		animatedSprites->PlayAnimation("death", 1, 3.f);
		deathtimer += dElapsedTime;
		break;
	}

	//CS: Update the animated sprite
	animatedSprites->Update(dElapsedTime);

	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, i32vec2Index.x, false, i32vec2NumMicroSteps.x*cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, i32vec2Index.y, false, i32vec2NumMicroSteps.y*cSettings->MICRO_STEP_YAXIS);
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
	glm::vec2 offset = glm::i32vec2((cSettings->NUM_TILES_XAXIS / 2) - 1, (cSettings->NUM_TILES_YAXIS / 2) - 1);
	glm::vec2 cameraPos = camera->getCurrPos();

	glm::vec2 IndexPos = (glm::vec2)i32vec2Index;
	IndexPos.x += ((float)i32vec2NumMicroSteps.x / cSettings->NUM_STEPS_PER_TILE_XAXIS) + 0.5f;
	IndexPos.y += ((float)i32vec2NumMicroSteps.y / cSettings->NUM_STEPS_PER_TILE_YAXIS) + 0.5f;

	glm::vec2 actualPos = IndexPos - cameraPos + offset;
	actualPos = cSettings->ConvertIndexToUVSpace(actualPos);

	transform = glm::translate(transform, glm::vec3(actualPos.x, actualPos.y, 0.f));

	//ORIGINAL TRANSFORM
	//transform = glm::translate(transform, glm::vec3(vec2UVCoordinate.x, vec2UVCoordinate.y, 0.0f));

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

/**
 @brief Constraint the player's position within a boundary
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
void CPlayer2D::Constraint(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		if (i32vec2Index.x < 0)
		{
			i32vec2Index.x = 0;
			i32vec2NumMicroSteps.x = 0;
		}
	}
	else if (eDirection == RIGHT)
	{
		if (i32vec2Index.x >= cMap2D->GetLevelCol() - 1)
		{
			i32vec2Index.x = cMap2D->GetLevelCol() - 1;
			i32vec2NumMicroSteps.x = 0;
		}
	}
	else if (eDirection == UP)
	{
		if (i32vec2Index.y >= cMap2D->GetLevelRow() - 1)
		{
			i32vec2Index.y = cMap2D->GetLevelRow() - 1;
			i32vec2NumMicroSteps.y = 0;
		}
	}
	else if (eDirection == DOWN)
	{
		if (i32vec2Index.y < 0)
		{
			i32vec2Index.y = 0;
			i32vec2NumMicroSteps.y = 0;
		}
	}
	else
	{
		cout << "CPlayer2D::Constraint: Unknown direction." << endl;
	}
}

/**
 @brief Check if a position is possible to move into
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
bool CPlayer2D::CheckPosition(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		// If the new position is fully within a row, then check this row only
		if (i32vec2NumMicroSteps.y == 0)
		{
			bool Active_0 = cMap2D->GetMapActive(i32vec2Index.y, i32vec2Index.x);
			int tileID_0 = cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x);
			// If the grid is not accessible, then return false
			if (tileID_0 >= 100 && Active_0 == true)
			{
				return false;
			}
		}
		// If the new position is between 2 rows, then check both rows as well
		else if (i32vec2NumMicroSteps.y != 0)
		{
			bool Active_0 = cMap2D->GetMapActive(i32vec2Index.y, i32vec2Index.x);
			int tileID_0 = cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x);

			bool Active_1 = cMap2D->GetMapActive(i32vec2Index.y + 1, i32vec2Index.x);
			int tileID_1 = cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x);
			// If the 2 grids are not accessible, then return false
			if ((tileID_0 >= 100 && Active_0 == true) ||
				(tileID_1 >= 100 && Active_1 == true))
			{
				return false;
			}
		}
	}
	else if (eDirection == RIGHT)
	{
		// If the new position is at the top row, then return true
		if (i32vec2Index.x >= cMap2D->GetLevelCol()- 1)
		{
			i32vec2NumMicroSteps.x = 0;
			return true;
		}

		// If the new position is fully within a row, then check this row only
		if (i32vec2NumMicroSteps.y == 0)
		{
			bool Active_0 = cMap2D->GetMapActive(i32vec2Index.y, i32vec2Index.x + 1);
			int tileID_0 = cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1);

			// If the grid is not accessible, then return false
			if (tileID_0 >= 100 && Active_0 == true)
			{
				return false;
			}
		}
		// If the new position is between 2 rows, then check both rows as well
		else if (i32vec2NumMicroSteps.y != 0)
		{
			bool Active_0 = cMap2D->GetMapActive(i32vec2Index.y, i32vec2Index.x + 1);
			int tileID_0 = cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1);

			bool Active_1 = cMap2D->GetMapActive(i32vec2Index.y + 1, i32vec2Index.x + 1);
			int tileID_1 = cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1);

			// If the 2 grids are not accessible, then return false
			if ((tileID_0 >= 100 && Active_0 == true) ||
				(tileID_1 >= 100 && Active_1 == true))
			{
				return false;
			}
		}

	}
	else if (eDirection == UP)
	{
		// If the new position is at the top row, then return true
		if (i32vec2Index.y >= cMap2D->GetLevelRow() - 1)
		{
			i32vec2NumMicroSteps.y = 0;
			return true;
		}

		// If the new position is fully within a column, then check this column only
		if (i32vec2NumMicroSteps.x == 0)
		{
			bool Active_0 = cMap2D->GetMapActive(i32vec2Index.y + 1, i32vec2Index.x);
			int tileID_0 = cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x);
			// If the grid is not accessible, then return false
			if (tileID_0 >= 100 && Active_0 == true)
			{
				return false;
			}
		}
		// If the new position is between 2 columns, then check both columns as well
		else if (i32vec2NumMicroSteps.x != 0)
		{
			bool Active_0 = cMap2D->GetMapActive(i32vec2Index.y + 1, i32vec2Index.x);
			int tileID_0 = cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x);

			bool Active_1 = cMap2D->GetMapActive(i32vec2Index.y + 1, i32vec2Index.x + 1);
			int tileID_1 = cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1);
			// If the 2 grids are not accessible, then return false
			if ((tileID_0 >= 100 && Active_0 == true) ||
				(tileID_1 >= 100 && Active_1 == true))
			{
				return false;
			}
		}
	}
	else if (eDirection == DOWN)
	{
		// If the new position is fully within a column, then check this column only
		if (i32vec2NumMicroSteps.x == 0)
		{
			bool Active_0 = cMap2D->GetMapActive(i32vec2Index.y, i32vec2Index.x);
			int tileID_0 = cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x);
			// If the grid is not accessible, then return false
			if (tileID_0 >= 100 && Active_0 == true)
			{
				return false;
			}
		}
		// If the new position is between 2 columns, then check both columns as well
		else if (i32vec2NumMicroSteps.x != 0)
		{
			bool Active_0 = cMap2D->GetMapActive(i32vec2Index.y, i32vec2Index.x);
			int tileID_0 = cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x);

			bool Active_1 = cMap2D->GetMapActive(i32vec2Index.y, i32vec2Index.x + 1);
			int tileID_1 = cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1);
			// If the 2 grids are not accessible, then return false
			if ((tileID_0 >= 100 && Active_0 == true) ||
				(tileID_1 >= 100 && Active_1 == true))
			{
				return false;
			}
		}
	}
	else
	{
		cout << "CPlayer2D::CheckPosition: Unknown direction." << endl;
	}

	return true;
}

// Check if the player is in mid-air
bool CPlayer2D::IsMidAir(void)
{
	// if the player is at the bottom row, then he is not in mid-air for sure
	if (i32vec2Index.y == 0)
		return false;

	// Check if the tile below the player's current position is empty
	if (i32vec2NumMicroSteps.x > 0)
	{
		if ((cMap2D->GetMapInfo(i32vec2Index.y - 1, i32vec2Index.x + 1) == 0))
		{
			return true; 
		}
		if (cMap2D->GetMapInfo(i32vec2Index.y - 1, i32vec2Index.x + 1) < 100 || cMap2D->GetMapActive(i32vec2Index.y - 1, i32vec2Index.x + 1) == false)
		{
			return true;
		}
	}
	else
	{
		if ((cMap2D->GetMapInfo(i32vec2Index.y - 1, i32vec2Index.x) == 0))
		{
			return true;
		}
		if (cMap2D->GetMapInfo(i32vec2Index.y - 1, i32vec2Index.x) < 100 || cMap2D->GetMapActive(i32vec2Index.y - 1, i32vec2Index.x) == false)
		{
			return true;
		}
	}

	return false;
}

// Update Jump or Fall
void CPlayer2D::UpdateJumpFall(const double dElapsedTime)
{
	if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP)
	{
		// Update the elapsed time to the physics engine
		cPhysics2D.AddElapsedTime((float)dElapsedTime);
		// Call the physics engine update method to calculate the final velocity and displacement
		cPhysics2D.Update();
		// Get the displacement from the physics engine
		glm::vec2 v2Displacement = cPhysics2D.GetDeltaDisplacement();
		// Store the current i32vec2Index.y
		int iIndex_YAxis_OLD = i32vec2Index.y;

		int iDisplacement_MicroSteps = (int)(v2Displacement.y / cSettings->MICRO_STEP_YAXIS); //DIsplacement divide by distance for 1 microstep
		if (i32vec2Index.y < cMap2D->GetLevelRow())
		{
			i32vec2NumMicroSteps.y += iDisplacement_MicroSteps;
			if (i32vec2NumMicroSteps.y > cSettings->NUM_STEPS_PER_TILE_YAXIS)
			{
				i32vec2NumMicroSteps.y -= cSettings->NUM_STEPS_PER_TILE_YAXIS;
				if (i32vec2NumMicroSteps.y < 0)
					i32vec2NumMicroSteps.y = 0;
				i32vec2Index.y++;
			}
		}

		// Constraint the player's position within the screen boundary
		Constraint(UP);

		// Iterate through all rows until the proposed row
		// Check if the player will hit a tile; stop jump if so.
		int iIndex_YAxis_Proposed = i32vec2Index.y;
		for (int i = iIndex_YAxis_OLD; i <= iIndex_YAxis_Proposed; i++)
		{
			// Change the player's index to the current i value
			i32vec2Index.y = i;
			// If the new position is not feasible, then revert to old position
			if (CheckPosition(UP) == false)
			{
				// Align with the row
				i32vec2NumMicroSteps.y = 0;
				// Set the Physics to fall status
				cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
				break;
			}
		}

		// If the player is still jumping and the initial velocity has reached zero or below zero, 
		// then it has reach the peak of its jump
		if ((cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP) && (cPhysics2D.GetDeltaDisplacement().y <= 0.0000f))
		{
			float velX = cPhysics2D.GetInitialVelocity().x;
			// Set status to fall
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
			cPhysics2D.SetInitialVelocity(glm::vec2(velX, 0));
		}
	}
	else if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::FALL)
	{
		// Update the elapsed time to the physics engine
		cPhysics2D.AddElapsedTime((float)dElapsedTime);
		// Call the physics engine update method to calculate the final velocity and displacement
		cPhysics2D.Update();
		// Get the displacement from the physics engine
		glm::vec2 v2Displacement = cPhysics2D.GetDeltaDisplacement();

		// Store the current i32vec2Index.y
		int iIndex_YAxis_OLD = i32vec2Index.y;

		// Translate the displacement from pixels to indices
		int iDisplacement_MicroSteps = (int)(v2Displacement.y / cSettings->MICRO_STEP_YAXIS);

		if (i32vec2Index.y >= 0)
		{
			i32vec2NumMicroSteps.y -= fabs(iDisplacement_MicroSteps);
			if (i32vec2NumMicroSteps.y < 0)
			{
				i32vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
				i32vec2Index.y--;
			}
		}

		// Constraint the player's position within the screen boundary
		Constraint(DOWN);

		// Iterate through all rows until the proposed row
		// Check if the player will hit a tile; stop fall if so.
		int iIndex_YAxis_Proposed = i32vec2Index.y;
		for (int i = iIndex_YAxis_OLD; i >= iIndex_YAxis_Proposed; i--)
		{
			// Change the player's index to the current i value
			i32vec2Index.y = i;
			// If the new position is not feasible, then revert to old position
			if (CheckPosition(DOWN) == false)
			{
				// Revert to the previous position
				if (i != iIndex_YAxis_OLD)
					i32vec2Index.y = i + 1;
				// Set the Physics to idle status
				cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);
				state = S_IDLE;
				jumpCount = 0;
				i32vec2NumMicroSteps.y = 0;
				break;
			}
		}
	}

	if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP || cPhysics2D.GetStatus() == CPhysics2D::STATUS::FALL)
	{
		glm::vec2 v2Displacement = cPhysics2D.GetDeltaDisplacement();
		// Store the current i32vec2Index.y
		int iIndex_XAxis_OLD = i32vec2Index.x;

		int iDisplacement_MicroSteps = (int)(v2Displacement.x / cSettings->MICRO_STEP_YAXIS); //DIsplacement divide by distance for 1 microstep
		if (iDisplacement_MicroSteps < 0 && i32vec2Index.x >= 0) //Moving Left
		{
			i32vec2NumMicroSteps.x -= fabs(iDisplacement_MicroSteps);
			if (i32vec2NumMicroSteps.x < 0)
			{
				i32vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
				i32vec2Index.x--;
			}
			Constraint(LEFT);
			// Check if the player will hit a tile; stop jump if so.
			int iIndex_XAxis_Proposed = i32vec2Index.x;
			for (int i = iIndex_XAxis_OLD; i >= iIndex_XAxis_Proposed; i--)
			{
				// Change the player's index to the current i value
				i32vec2Index.x = i;
				// If the new position is not feasible, then revert to old position
				if (CheckPosition(LEFT) == false)
				{
					cPhysics2D.SetStatus(CPhysics2D::STATUS::HOLD);

					if (i != iIndex_XAxis_OLD)
						i32vec2Index.x = i + 1;
					jumpCount = 0;
					i32vec2NumMicroSteps.x = 0;
					break;
				}
			}
		}
		else if (iDisplacement_MicroSteps > 0 && i32vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
		{
			i32vec2NumMicroSteps.x += iDisplacement_MicroSteps;
			if (i32vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
			{
				i32vec2NumMicroSteps.x -= cSettings->NUM_STEPS_PER_TILE_XAXIS - 1;
				if (i32vec2NumMicroSteps.x < 0)
					i32vec2NumMicroSteps.x = 0;
				i32vec2Index.x++;
			}

			Constraint(RIGHT);
			// Check if the player will hit a tile; stop jump if so.
			int iIndex_XAxis_Proposed = i32vec2Index.x;
			for (int i = iIndex_XAxis_OLD; i <= iIndex_XAxis_Proposed; i++)
			{
				// Change the player's index to the current i value
				i32vec2Index.x = i;
				// If the new position is not feasible, then revert to old position
				if (CheckPosition(RIGHT) == false)
				{
					cPhysics2D.SetStatus(CPhysics2D::STATUS::HOLD);
					jumpCount = 0;
					i32vec2NumMicroSteps.x = 0;
					break;
				}
			}
		}
	}
}

/**
 @brief Let player interact with the map. You can add collectibles such as powerups and health here.
 */
void CPlayer2D::InteractWithMap(void)
{
	if (cKeyboardController->IsKeyPressed(GLFW_KEY_E) || cKeyboardController->IsKeyPressed(GLFW_KEY_F))
	{
		//Switch_1_Off
		if (InRangeOfTile(11))
		{
			cSoundController->PlaySoundByID(7);
			if(i32vec2NumMicroSteps.x > 0 && cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) == 11)
				cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 12);
			else if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) == 11)
				cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 12);
		}
		//Switch_1_On
		else if (InRangeOfTile(12))
		{
			cSoundController->PlaySoundByID(7);
			if (i32vec2NumMicroSteps.x > 0 && cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) == 12)
				cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 11);
			else if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) == 12)
				cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 11);
		}
		//Switch_2_Off
		else if (InRangeOfTile(13))
		{
			cSoundController->PlaySoundByID(7);
			if (i32vec2NumMicroSteps.x > 0 && cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) == 13)
				cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 14);
			else if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) == 13)
				cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 14);
		}
		//Switch_2_On
		else if (InRangeOfTile(14))
		{
			cSoundController->PlaySoundByID(7);
			if (i32vec2NumMicroSteps.x > 0 && cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) == 14)
				cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 13);
			else if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) == 14)
				cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 13);
		}
	}

	if (cKeyboardController->IsKeyPressed(GLFW_KEY_W))
	{
		//portal paired with 16
		if (InRangeOfTile(15))
		{
			unsigned xCoord = 0, yCoord = 0;
			cMap2D->FindValue(16, true, yCoord, xCoord);

			i32vec2Index = glm::vec2(xCoord, yCoord);
			i32vec2NumMicroSteps = glm::vec2(0.f);
		}
		//portal paired with 15
		else if (InRangeOfTile(16))
		{
			unsigned xCoord = 0, yCoord = 0;
			cMap2D->FindValue(15, true, yCoord, xCoord);

			i32vec2Index = glm::vec2(xCoord, yCoord);
			i32vec2NumMicroSteps = glm::vec2(0.f);
		}
		else if (InRangeOfTile(17)) //Level completion portalgate
		{
			CGameManager::GetInstance()->bLevelCompleted = true;
		}
	}
			
	switch (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x))
	{
	case 2:
		// Erase the tree from this position
		cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
		// Increase the Tree by 1
		cInventoryItem = cInventoryManager->GetItem("Shuriken");
		cInventoryItem->Add(1);
		// Play a bell sound
		cSoundController->PlaySoundByID(1);
		break;
	case 10:
		// Increase the lives by 1
		cInventoryItem = cInventoryManager->GetItem("Lives");
		cInventoryItem->Add(1);
		// Erase the life from this position
		cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
		break;
	case 20:
		// Decrease the health by 1
		cInventoryItem = cInventoryManager->GetItem("Health");
		Hit(10);
		break;
	case 21:
		// Increase the health
		cInventoryItem = cInventoryManager->GetItem("Health");
		cInventoryItem->Add(1);
		break;
	default:
		break;
	}
}

/**
 @brief Update the health and lives.
 */
void CPlayer2D::UpdateHealthLives(void)
{
	// Update health and lives
	cInventoryItem = cInventoryManager->GetItem("Health");
	// Check if a life is lost
	if (cInventoryItem->GetCount() <= 0)
	{
		state = S_DEATH;
		// Reset the Health to max value
		cInventoryItem->iItemCount = cInventoryItem->GetMaxCount();
		// But we reduce the lives by 1.
		cInventoryItem = cInventoryManager->GetItem("Lives");
		cInventoryItem->Remove(1);
		cSoundController->PlaySoundByID(9);

		// Check if there is no lives left...
		if (cInventoryItem->GetCount() < 0)
		{
			// Player loses the game
			CGameManager::GetInstance()->bPlayerLost = true;
		}
	}
}

void CPlayer2D::Hit(int health)
{
	if (!bDamaged && state != S_DEATH)
	{
		cInventoryItem = cInventoryManager->GetItem("Health");
		cInventoryItem->Remove(health);
	}
	bDamaged = true;
}

bool CPlayer2D::InRangeOfTile(unsigned tileID)
{
	if ((i32vec2NumMicroSteps.x > 0 && cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) == tileID))
	{
		return true;
	}
	else if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) == tileID)
	{
		return true;
	}

	return false;
}
