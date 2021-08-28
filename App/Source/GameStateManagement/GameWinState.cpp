#include "GameWinState.h"

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

GameWinState::GameWinState(void)
 : background(NULL)
	,fUp(0.f)

{
}

GameWinState::~GameWinState(void)
{
}

bool GameWinState::Init(void)
{
	cout << "GameWinState::Init()\n" << endl;

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

	CSoundController::GetInstance()->PlaySoundByID(SOUND_ID::SOUND_GAME_COMPLETE);
	return true;
}

bool GameWinState::Update(const double dElapsedTime)
{
	fUp += 1.f;
	cout << "Up value is " << fUp << endl;
	//cout << "Fade count = " << iFadeCount2 << endl;
	//out << "Fade count = " << iFadeCount << endl;
	//For keyboard controls
	//if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_SPACE) ||
	//	CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE))
	//{
	//	// Reset the CKeyboardController
	//	CKeyboardController::GetInstance()->Reset();

	//	// Load the menu state
	//	cout << "Quitting the game from GameWinState" << endl;
	//	return false;
	//}

	return true;
}

void GameWinState::Render(void)
{
	// Clear the screen and buffer
	glClearColor(0.0f, 0.55f, 1.00f, 1.00f);

	//Render Background
	background->Render();
}

void GameWinState::Destroy(void)
{
	// Delete the background
	if (background)
	{
		delete background;
		background = NULL;
	}

	cout << "CMenuState::Destroy()\n" << endl;
}

bool GameWinState::ImGuiRender()
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

		ImGui::Begin("You Won screen", NULL, window_flags);
		ImGui::SetWindowSize(ImVec2(1200.0f * relativeScale_x, 200.0f * relativeScale_y));
		ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.37f,cSettings->iWindowHeight * 0.4f - fUp));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.0f));
		ImGui::Text("Congratulations");
		ImGui::Text("   You won!");
		ImGui::SetWindowFontScale(relativeScale_x + relativeScale_y);
		ImGui::PopStyleColor();
		//ImGui::Image((void*)(intptr_t)gameOverImage.textureID, ImVec2(buttonWidth, buttonWidth));
		ImGui::End();

		ImGui::Begin("Credit page", NULL, window_flags);
		ImGui::SetWindowSize(ImVec2(1200.0f * relativeScale_x, 1200.0f * relativeScale_y));
		ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.37f, cSettings->iWindowHeight * 1.2f - fUp));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.0f, 1.0f, 1.0f));
		ImGui::Text("Made by : ");
		ImGui::Text("");
		ImGui::Text("Jeryl Lah (191019G)");
		ImGui::Text("");
		ImGui::Text("");
		ImGui::Text("Joash Foo (200528H)"); 
		ImGui::Text("");
		ImGui::Text("");
		ImGui::Text("Jevon Liao (191126T)");
		ImGui::Text("");
		ImGui::Text("");
		ImGui::Text("Sean Lee (200372B)");
		ImGui::SetWindowFontScale(relativeScale_x + relativeScale_y);
		ImGui::PopStyleColor();
		//ImGui::Image((void*)(intptr_t)gameOverImage.textureID, ImVec2(buttonWidth, buttonWidth));
		ImGui::End();


	}

	return true;
}

