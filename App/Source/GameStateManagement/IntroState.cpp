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
#include "SoundController/SoundController.h"
#include "System/filesystem.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CIntroState::CIntroState(void)
	: background(NULL)
	, countdown(0)
	, currentColor(1, 1, 1, 0)
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

	//Create Background Entity
	background = new CBackgroundEntity("Image/Backgrounds/Intro.png");
	background->SetShader("2DColorShader");
	// Include Shader Manager
	CShaderManager::GetInstance()->Use("2DColorShader");
	CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);
	CShaderManager::GetInstance()->activeShader->setVec4("runtime_color", currentColor);

	background->Init();

	//Sound effects
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Sound_Intro.ogg"), SOUND_ID::SOUND_INTRO, true, false, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Sound_Bell.ogg"), SOUND_ID::SOUND_BELL, true, false, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Sound_Explosion.ogg"), SOUND_ID::SOUND_EXPLOSION, true, false, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Sound_Jump.ogg"), SOUND_ID::SOUND_JUMP, true, false, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Swing.ogg"), SOUND_ID::SOUND_SWING, true, false, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Switch.ogg"), SOUND_ID::SOUND_SWITCH, true, false, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\GameComplete.ogg"), SOUND_ID::SOUND_GAME_COMPLETE, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Dead.ogg"), SOUND_ID::SOUND_DEATH, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Click.ogg"), SOUND_ID::SOUND_ONCLICK, true, false, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Teleporter.ogg"), SOUND_ID::SOUND_TELEPORTER, true, false, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\GotHit.ogg"), SOUND_ID::SOUND_HIT, true, false, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Potion.ogg"), SOUND_ID::SOUND_POTION, true, false, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Door_Open.ogg"), SOUND_ID::SOUND_DOOR_OPEN, true, false, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Door_Close.ogg"), SOUND_ID::SOUND_DOOR_CLOSE, true, false, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Sound_Chest.ogg"), SOUND_ID::SOUND_CHEST, true, false, true);

	//BGM
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Henesys_BGM.ogg"), SOUND_ID::BGM_HENESYS, true, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Level1_BGM.ogg"), SOUND_ID::BGM_LEVEL1, true, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\2nd_BGM.ogg"), SOUND_ID::BGM_SECOND, true, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Trouble.ogg"), SOUND_ID::BGM_TROUBLE, true);
	CSoundController::GetInstance()->LoadSound(FileSystem::getPath("Sounds\\Win.ogg"), SOUND_ID::BGM_WIN, true);
	CSettings::GetInstance()->UpdateSoundSettings();

	CSoundController::GetInstance()->PlaySoundByID(SOUND_ID::SOUND_INTRO);
	return true;
}

/**
 @brief Update this class instance
 */
bool CIntroState::Update(const double dElapsedTime)
{
	countdown += dElapsedTime;

	if (countdown >= 5)
	{
		currentColor -= glm::vec4(0, 0, 0,  0.5 * dElapsedTime);
	}
	else if (countdown <= 2)
	{
		currentColor += glm::vec4(0, 0, 0,  0.5 * dElapsedTime);
	}

	currentColor = glm::clamp(currentColor, glm::vec4(0, 0, 0, 0), glm::vec4(1, 1, 1, 1));

	if (countdown > 8)
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Clear the screen and buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	CShaderManager::GetInstance()->Use("2DColorShader");
	CShaderManager::GetInstance()->activeShader->setVec4("runtime_color", currentColor);

	//Draw the background
 	background->Render();

	glDisable(GL_BLEND);
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

