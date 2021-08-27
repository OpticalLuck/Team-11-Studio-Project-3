#include "Scene2D.h"
#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

#include "System\filesystem.h"
#include "Math/MyMath.h"

//Enemy shit
#include "MobEnemy2D.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CScene2D::CScene2D(void)
	: cMap2D(NULL)
	, cPlayer2D(NULL)
	, cKeyboardController(NULL)
	, cGameManager(NULL)
	, cSoundController(NULL)
	, CInputHandler(NULL)
	, isCompleted(false)
	, cEntityManager(NULL)
	, cameraHandler(NULL)
	, cInventoryM(NULL)
	, transform(glm::mat4(1))
	, dParadoxiumTimer(10.f)
{
}

/**
 @brief Destructor
 */
CScene2D::~CScene2D(void)
{
	if (cSoundController)
	{
		cSoundController->StopSoundByID(BGM_HENESYS);
		// We won't delete this since it was created elsewhere
		cSoundController = NULL;
	}

	if (cGameManager)
	{
		cGameManager->Destroy();
		cGameManager = NULL;
	}

	// We won't delete this since it was created elsewhere
	cKeyboardController = NULL;

	CInputHandler = NULL;

	// Destroy the enemies
	cEntityManager->Clear();

	if (cMap2D)
	{
		cMap2D->Destroy();
		cMap2D = NULL;
	}

	if (cameraHandler) {
		cameraHandler->Destroy();
		cameraHandler = NULL;
	}
}

void CScene2D::StartParadoxiumAbility()
{
	std::vector<CEnemy2D*> enemyArr = cEntityManager->GetAllEnemies();

	cPlayer2D->SetRecording(true);
	cPlayer2D->m_FrameStorage.iStoredFrame = cPlayer2D->m_FrameStorage.iCurrentFrame;
	cPlayer2D->m_FrameStorage.spawnPos = cPlayer2D->vTransform;
	//Recording enemy stuff
	CEntity2D::SetRecording(true);
	for (unsigned i = 0; i < enemyArr.size(); i++) {
		enemyArr[i]->ResetRecording();
	}

	//Saving of other movable entities
	cEntityManager->SavePrevious();
}

void CScene2D::StopParadoxiumAbility()
{
	std::vector<CEnemy2D*> enemyArr = cEntityManager->GetAllEnemies();

	cPlayer2D->SetRecording(false);
	CPlayer2D* clone = CEntityManager::GetInstance()->Clone();
	//Loading of clone
	clone->vTransform = cPlayer2D->m_FrameStorage.spawnPos;
	clone->m_FrameStorage.iCurrentFrame = cPlayer2D->m_FrameStorage.iStoredFrame;
	clone->m_FrameStorage.iEndFrame = cPlayer2D->m_FrameStorage.iCurrentFrame;
	cPlayer2D->m_FrameStorage.iStoredFrame = 0;
	cPlayer2D->vTransform = cPlayer2D->m_FrameStorage.spawnPos;
	clone->SetHealth(cEntityManager->GetPlayer()->GetHealth());

	cPlayer2D->m_FrameStorage.iCounter = 0;

	//Recording enemy stuff
	CEntity2D::SetRecording(false);

	for (unsigned i = 0; i < enemyArr.size(); i++) {
		enemyArr[i]->ReplayRecording();
	}

	//Loading prev values of movable entities
	cEntityManager->LoadPrevious();

	dParadoxiumTimer = 10.f;
}

/**
@brief Init Initialise this instance
*/ 
bool CScene2D::Init(std::string levelPath)
{
	Math::InitRNG();
	// Include Shader Manager
	CShaderManager::GetInstance()->Use("2DShader");
	CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);

	// Create and initialise the Map 2D
	cMap2D = CMap2D::GetInstance();
	// Set a shader to this class
	cMap2D->SetShader("2DShader");
	// Initialise the instance
	if (cMap2D->Init(3, 24, 32) == false)
	{
		cout << "Failed to load CMap2D" << endl;
		return false;
	}
	// Load the map into an array
	if (cMap2D->LoadMap(levelPath, 0) == false)
	{
		// The loading of a map has failed. Return false
		return false;
	}

	// Load Scene2DColor into ShaderManager
	CShaderManager::GetInstance()->Add("2DColorShader", "Shader//Scene2DColor.vs", "Shader//Scene2DColor.fs");
	CShaderManager::GetInstance()->Use("2DColorShader");
	CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);
	cEntityManager = CEntityManager::GetInstance();
	cEntityManager->InitPlayer();

	cPlayer2D = cEntityManager->GetPlayer();

	cameraHandler = Camera2D::GetInstance();
	cameraHandler->Reset();
	cameraHandler->UpdateTarget(cPlayer2D->vTransform);
	
	//300
	//LoadEnemy<CEnemy2D>();

	// Setup the shaders
	CShaderManager::GetInstance()->Add("textShader", "Shader//text.vs", "Shader//text.fs");
	CShaderManager::GetInstance()->Use("textShader");

	// Store the keyboard controller singleton instance here
	cKeyboardController = CKeyboardController::GetInstance();

	// Game Manager
	cGameManager = CGameManager::GetInstance();
	cGameManager->Init();

	// Load the sounds into CSoundController
	cSoundController = CSoundController::GetInstance();
	cSoundController->PlaySoundByID(SOUND_ID::BGM_HENESYS);
	//CSettings::SongPlaylist();

	CInputHandler = CInputHandler::GetInstance();

	//cInventoryManager = CInventoryManager::GetInstance();
	cInventoryM = CInventoryManager::GetInstance();
		
	return true;
}

/**
@brief Update Update this instance
*/
bool CScene2D::Update(const double dElapsedTime)
{

	cEntityManager->Update(dElapsedTime);

	// Call the Map2D's update method
	cMap2D->Update(dElapsedTime);

	if (CMouseController::GetInstance()->GetMouseScrollStatus(CMouseController::SCROLL_TYPE_YOFFSET) > 0)
	{
		Camera2D::GetInstance()->UpdateZoom(Camera2D::GetInstance()->getTargetZoom() + 0.1f);
	}
	if (CMouseController::GetInstance()->GetMouseScrollStatus(CMouseController::SCROLL_TYPE_YOFFSET) < 0)
	{
		Camera2D::GetInstance()->UpdateZoom(Camera2D::GetInstance()->getTargetZoom() - 0.1f);
	}

	if (fCooldown > 0)
	{
		fCooldown -= (float)dElapsedTime;
	}

	// Get keyboard updates
	if (cKeyboardController->IsKeyDown(GLFW_KEY_F6))
	{
		// Save the current game to a save file
		// Make sure the file is open
		try {
			if (cMap2D->SaveMap("Maps/DM2213_Map_Level_01_SAVEGAMEtest.csv") == false)
			{
				throw runtime_error("Unable to save the current game to a file");
			}
		}
		catch (runtime_error e)
		{
			cout << "Runtime error: " << e.what();
			return false;
		}
	}

	// Paradoxium ability
	if (cKeyboardController->IsKeyPressed(GLFW_KEY_C) && cEntityManager->GetNoOfAvailableCloneID() > 0)
	{
		++cPlayer2D->m_FrameStorage.iCounter;
		switch (cPlayer2D->m_FrameStorage.iCounter)
		{
		case 1:
			StartParadoxiumAbility();
			break;
		case 2:

			StopParadoxiumAbility();
			break;
		}
	}

	if (cPlayer2D->m_FrameStorage.iCounter == 1)
	{
		if (dParadoxiumTimer <= 0.f)
		{
			StopParadoxiumAbility();
		}
		dParadoxiumTimer -= dElapsedTime;
	}


	//Camera work
	cameraHandler->UpdateTarget(cPlayer2D->vTransform);
	cameraHandler->Update((float)dElapsedTime);
	cameraHandler->ClampCamPos(cMap2D->GetLevelLimit());

	// Check if the game should go to the next level
	if (cGameManager->bLevelCompleted == true)
	{
		if (cMap2D->GetCurrentLevel() + 1 < 3)
		{
			cMap2D->SetCurrentLevel(cMap2D->GetCurrentLevel() + 1);
			cPlayer2D->Reset();

			//300
			//LoadEnemy<CEnemy2D>();
		}
		//Last Level
		else
		{
			cGameManager->bPlayerWon = true;
		}
		cGameManager->bLevelCompleted = false;
	}

	// Check if the game has been won by the player
	if (cGameManager->bPlayerWon == true)
	{
		// End the game and switch to Win screen
		isCompleted = true;
	}
	// Check if the game should be ended
	else if (cGameManager->bPlayerLost == true)
	{
		cSoundController->PlaySoundByID(SOUND_ID::SOUND_EXPLOSION);
		return false;
	}
	if (cPlayer2D->GetHealth() < 3) // change bgm if the health is dying 
	{
		if (!cSoundController->isCurrentlyPlaying(cSoundController->GetNamebyID(SOUND_ID::SOUND_TROUBLE)))
		{
			cSoundController->StopPlayBack();
			cSoundController->PlaySoundByID(SOUND_ID::SOUND_TROUBLE);
		}
	}
	return true;
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CScene2D::PreRender(void)
{
	// Reset the OpenGL rendering environment
	glLoadIdentity();

	// Clear the screen and buffer
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Enable 2D texture rendering
	glEnable(GL_TEXTURE_2D);
}

/**
 @brief Render Render this instance
 */
void CScene2D::Render(void)
{
	//Call the Map's background (If there's any)
	cMap2D->RenderBackground();

	// Call the Map2D's PreRender()
	cMap2D->PreRender();
	// Call the Map2D's Render()
	cMap2D->Render();
	// Call the Map2D's PostRender()
	cMap2D->PostRender();

	cEntityManager->RenderInteractables();
	cEntityManager->RenderBullets();
	cEntityManager->RenderEnemy();
	cEntityManager->RenderClone();
	cEntityManager->RenderPlayer();
	cEntityManager->RenderObstacles();
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CScene2D::PostRender(void)
{
}



