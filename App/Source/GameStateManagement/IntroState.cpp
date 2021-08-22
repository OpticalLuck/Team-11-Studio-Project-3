// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <Windows.h>
#include <GLFW/glfw3.h>

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include "IntroState.h"

// Include CGameStateManager
#include "GameStateManager.h"

// Include Mesh Builder
#include "Primitives/MeshBuilder.h"
// Include ImageLoader
#include "System\ImageLoader.h"
// Include Shader Manager
#include "RenderControl\ShaderManager.h"

 // Include shader
#include "RenderControl\shader.h"

// Include CSettings
#include "GameControl/Settings.h"

// Include CKeyboardController
#include "Inputs/KeyboardController.h"
#include "../App/Source/SoundController/SoundController.h"
#include "System/filesystem.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CIntroState::CIntroState(void)
	: background(NULL)
	, countdown(0)
{

}

/**
 @brief Destructor
 */
CIntroState::~CIntroState(void)
{
}

/**
 @brief Init this class instance
 */
bool CIntroState::Init(void)
{
	cout << "CIntroState::Init()\n" << endl;

	// Include Shader Manager
	CShaderManager::GetInstance()->Use("2DShader");
	CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);

	//Create Background Entity
	background = new CBackgroundEntity("Image/IntroBackground.png");
	background->SetShader("2DShader");
	background->Init();

	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Sound_Bell.ogg"), 1, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Sound_Explosion.ogg"), 2, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Sound_Jump.ogg"), 3, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Henesys_BGM.ogg"), 4, true, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Hit.ogg"), 5, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Swing.ogg"), 6, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Switch.ogg"), 7, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\GameComplete.ogg"), 8, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Dead.ogg"), 9, true);

	return true;
}

/**
 @brief Update this class instance
 */
bool CIntroState::Update(const double dElapsedTime)
{
	countdown += dElapsedTime;
	if (countdown > 1)
	{
		countdown = 0;
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "Loading MenuState" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("MenuState");
		return true;
	}

	return true;
}

/**
 @brief Render this class instance
 */
void CIntroState::Render()
{
	// Clear the screen and buffer
	glClearColor(0.0f, 0.55f, 1.00f, 1.00f);

	//Draw the background
 	background->Render();
}

/**
 @brief Destroy this class instance
 */
void CIntroState::Destroy(void)
{
	// Delete the background
	if (background)
	{
		delete background;
		background = NULL;
	}

	cout << "CIntroState::Destroy()\n" << endl;
}

bool CIntroState::ImGuiRender()
{
	return true;
}