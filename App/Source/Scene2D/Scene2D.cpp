#include "Scene2D.h"
#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

#include "System\filesystem.h"
#include "Math/MyMath.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CScene2D::CScene2D(void)
	: cMap2D(NULL)
	, cPlayer2D(NULL)
	, cKeyboardController(NULL)
	, cGUI_Scene2D(NULL)
	, cGameManager(NULL)
	, cSoundController(NULL)
	, isCompleted(false)
{
}

/**
 @brief Destructor
 */
CScene2D::~CScene2D(void)
{
	if (cSoundController)
	{
		// We won't delete this since it was created elsewhere
		cSoundController = NULL;
	}

	if (cGameManager)
	{
		cGameManager->Destroy();
		cGameManager = NULL;
	}

	if (cGUI_Scene2D)
	{
		cGUI_Scene2D->Destroy();
		cGUI_Scene2D = NULL;
	}

	// We won't delete this since it was created elsewhere
	cKeyboardController = NULL;

	// Destroy the enemies
	for (int i = 0; i < enemyVector.size(); i++)
	{
		delete enemyVector[i];
		enemyVector[i] = NULL;
	}
	enemyVector.clear();

	if (cPlayer2D)
	{
		cPlayer2D->Destroy();
		cPlayer2D = NULL;
	}

	if (cMap2D)
	{
		cMap2D->Destroy();
		cMap2D = NULL;
	}

	// Clear out all the shaders
	//CShaderManager::GetInstance()->Destroy();
}

/**
@brief Init Initialise this instance
*/ 
bool CScene2D::Init(void)
{
	Math::InitRNG();
	// Include Shader Manager
	//CShaderManager::GetInstance()->Add("2DShader", "Shader//Scene2D.vs", "Shader//Scene2D.fs");
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
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_01.csv") == false)
	{
		// The loading of a map has failed. Return false
		return false;
	}
	// Load the map into an array
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_02.csv", 1) == false)
	{
		// The loading of a map has failed. Return false
		return false;
	}
	// Load the map into an array
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_03.csv", 2) == false)
	{
		// The loading of a map has failed. Return false
		return false;
	}

	// Activate diagonal movement
	cMap2D->SetDiagonalMovement(false);

	// Load Scene2DColor into ShaderManager
	CShaderManager::GetInstance()->Add("2DColorShader", "Shader//Scene2DColor.vs", "Shader//Scene2DColor.fs");
	CShaderManager::GetInstance()->Use("2DColorShader");
	CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);
	// Create and initialise the CPlayer2D
	cPlayer2D = CPlayer2D::GetInstance();
	// Pass shader to cPlayer2D
	cPlayer2D->SetShader("2DColorShader");
	// Initialise the instance
	if (cPlayer2D->Init() == false)
	{
		cout << "Failed to load CPlayer2D" << endl;
		return false;
	}

	// Create and initialise the CEnemy2D
	enemyVector.clear();
	
	//300
	LoadEnemy<CEnemy2D>();

	// Setup the shaders
	CShaderManager::GetInstance()->Add("textShader", "Shader//text.vs", "Shader//text.fs");
	CShaderManager::GetInstance()->Use("textShader");

	// Store the keyboard controller singleton instance here
	cKeyboardController = CKeyboardController::GetInstance();

	// Store the cGUI_Scene2D singleton instance here
	cGUI_Scene2D = CGUI_Scene2D::GetInstance();
	cGUI_Scene2D->Init();

	// Game Manager
	cGameManager = CGameManager::GetInstance();
	cGameManager->Init();

	// Load the sounds into CSoundController
	cSoundController = CSoundController::GetInstance();
	cSoundController->PlaySoundByID(4);
	return true;
}

/**
@brief Update Update this instance
*/
bool CScene2D::Update(const double dElapsedTime)
{
	// Call the cPlayer2D's update method before Map2D as we want to capture the inputs before map2D update
	cPlayer2D->Update(dElapsedTime);

	// Call all the cEnemy2D's update method before Map2D 
	// as we want to capture the updates before map2D update
	for (int i = 0; i < enemyVector.size(); i++)
	{
		if (static_cast<CEnemy2D*>(enemyVector[i])->GetHealth() < 0)
		{
			delete enemyVector[i];
			enemyVector.erase(enemyVector.begin() + i);
		}
	}
	for (int i = 0; i < enemyVector.size(); i++)
	{
		enemyVector[i]->Update(dElapsedTime);
	}
	 
	// Call the Map2D's update method
	cMap2D->Update(dElapsedTime);

	// Get keyboard updates
	if (cKeyboardController->IsKeyDown(GLFW_KEY_F6))
	{
		cSoundController->PlaySoundByID(4);
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
	// Call the cGUI_Scene2D's update method
	cGUI_Scene2D->Update(dElapsedTime);

	// Check if the game should go to the next level
	if (cGameManager->bLevelCompleted == true)
	{
		if (cMap2D->GetCurrentLevel() + 1 < 3)
		{
			cMap2D->SetCurrentLevel(cMap2D->GetCurrentLevel() + 1);
			int initialsize = enemyVector.size();
			for (int i = 0; i < initialsize; i++)
			{
				delete enemyVector[0];
				enemyVector.erase(enemyVector.begin());
			}
			cPlayer2D->Reset();

			//300
			LoadEnemy<CEnemy2D>();
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
		cSoundController->PlaySoundByID(2);
		return false;
	}
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
	for (int i = 0; i < enemyVector.size(); i++)
	{
		// Call the CEnemy2D's PreRender()
		enemyVector[i]->PreRender();
		// Call the CEnemy2D's Render()
		enemyVector[i]->Render();
		// Call the CEnemy2D's PostRender()
		enemyVector[i]->PostRender();
	}

	// Call the Map2D's PreRender()
	cMap2D->PreRender();
	// Call the Map2D's Render()
	cMap2D->Render();
	// Call the Map2D's PostRender()
	cMap2D->PostRender();

	// Call the CPlayer2D's PreRender()
	cPlayer2D->PreRender();
	// Call the CPlayer2D's Render()
	cPlayer2D->Render();
	// Call the CPlayer2D's PostRender()
	cPlayer2D->PostRender();

	// Call the cGUI_Scene2D's PreRender()
	cGUI_Scene2D->PreRender();
	// Call the cGUI_Scene2D's Render()
	cGUI_Scene2D->Render();
	// Call the cGUI_Scene2D's PostRender()
	cGUI_Scene2D->PostRender();
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CScene2D::PostRender(void)
{
}

void CScene2D::CreateIMGUI()
{
	// Store the cGUI_Scene2D singleton instance here
	cGUI_Scene2D = CGUI_Scene2D::GetInstance();
	cGUI_Scene2D->CreateIMGUI();
}

void CScene2D::DeleteIMGUI()
{
	if (cGUI_Scene2D)
	{
		cGUI_Scene2D->DeleteIMGUI();
		cGUI_Scene2D = NULL;
	}
}
