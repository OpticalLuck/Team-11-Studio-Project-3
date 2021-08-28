#include "GameOverState.h"

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

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

#include <iostream>
using namespace std;

GameOverState::GameOverState(void)
 : background(NULL)
	,fFadeCount(0.f)
	,fFadeCount2(0.f)
	,fWaitTime(0.f)
{
}

GameOverState::~GameOverState(void)
{
}

bool GameOverState::Init(void)
{
	cout << "GameOverState::Init()\n" << endl;

	CShaderManager::GetInstance()->Use("2DShader");
	CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);

	//Create Background Entity
	background = new CBackgroundEntity("Image/Backgrounds/black.png");
	background->SetShader("2DShader");
	background->Init();

	cSettings = CSettings::GetInstance();
	// Load the images for buttons
	//CImageLoader* il = CImageLoader::GetInstance();
	//exitButtonData.fileName = "Image\\GUI\\ExitButton.png";
	//exitButtonData.textureID = il->LoadTextureGetID(exitButtonData.fileName.c_str(), false);

	//gameOverImage.fileName = "Image\\GUI\\gameover.png";
	//gameOverImage.textureID = il->LoadTextureGetID(gameOverImage.fileName.c_str(), false);


	fFadeCount = 0.f;
	bDecrease = true;
	
	fFadeCount2 = 0.f;
	bIncrease2 = true;
	bDecrease2 = false;
	
	fWaitTime = 0.f;
	bWaitTimeIncrease = true;

	CSoundController::GetInstance()->PlaySoundByID(SOUND_ID::SOUND_GAME_COMPLETE);
	return true;
}

bool GameOverState::Update(const double dElapsedTime)
{
	fWaitTime += 0.01f;
	if (fWaitTime >= 3.f)
	{
		//for game over timing
		{
			if (fFadeCount <= -1.f)
			{
				bDecrease = false;
			}
			if (bDecrease)
				fFadeCount -= 0.005f;
		}


		//for retry timing
		{
			if (fFadeCount < -1.f)
			{
				if (bIncrease2)
					fFadeCount2 += 0.01f;
				if (bDecrease2)
					fFadeCount2 -= 0.01f;
				if (fFadeCount2 <= -.1f)
				{
					bIncrease2 = true;
					bDecrease2 = false;
				}
				else if (fFadeCount2 >= 1.f)
				{
					bIncrease2 = false;
					bDecrease2 = true;
				}
				if(CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_SPACE))
					CGameStateManager::GetInstance()->SetActiveGameState("MenuState");
			}
		}
	}
	cout << "Wait time = " << fWaitTime << endl;
	//cout << "Fade count = " << iFadeCount2 << endl;
	//out << "Fade count = " << iFadeCount << endl;
	//For keyboard controls
	//if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_SPACE) ||
	//	CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE))
	//{
	//	// Reset the CKeyboardController
	//	CKeyboardController::GetInstance()->Reset();

	//	// Load the menu state
	//	cout << "Quitting the game from GameoverState" << endl;
	//	return false;
	//}

	return true;
}

void GameOverState::Render(void)
{
	// Clear the screen and buffer
	glClearColor(0.0f, 0.55f, 1.00f, 1.00f);

	//Render Background
	background->Render();
}

void GameOverState::Destroy(void)
{
	// Delete the background
	if (background)
	{
		delete background;
		background = NULL;
	}

	cout << "CMenuState::Destroy()\n" << endl;
}

bool GameOverState::ImGuiRender()
{

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	//window_flags |= ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoNav;

	float buttonWidth = 256;
	float buttonHeight = 128;

	float relativeScale_x = cSettings->iWindowWidth / 800.0f;
	float relativeScale_y = cSettings->iWindowHeight / 600.0f;

	relativeScale_x = Math::Max(1.f, relativeScale_x);
	relativeScale_y = Math::Max(1.f, relativeScale_y);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Game Over message", NULL, window_flags);
		ImGui::SetWindowSize(ImVec2(1200.0f * relativeScale_x, 200.0f * relativeScale_y));
		ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.37f,cSettings->iWindowHeight * 0.4f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, .0f, 0.0f, 1.0f + fFadeCount));
		ImGui::Text("You Died");
		ImGui::SetWindowFontScale(2 * (relativeScale_x + relativeScale_y));
		ImGui::PopStyleColor();
		//ImGui::Image((void*)(intptr_t)gameOverImage.textureID, ImVec2(buttonWidth, buttonWidth));
		ImGui::End();

		ImGui::Begin("Retry message", NULL, window_flags);
		ImGui::SetWindowSize(ImVec2(1200.0f * relativeScale_x, 200.0f * relativeScale_y));
		ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.17f, cSettings->iWindowHeight * 0.4f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, .0f, 0.0f, .0f + fFadeCount2));
		ImGui::Text("Press SpaceBar to retry");
		ImGui::SetWindowFontScale(2 * (relativeScale_x + relativeScale_y));
		ImGui::PopStyleColor();
		//ImGui::Image((void*)(intptr_t)gameOverImage.textureID, ImVec2(buttonWidth, buttonWidth));
		ImGui::End();


	}

	return true;
}

