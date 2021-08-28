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
	,fDown(0.f)
	,fLeft(0.f)
	,fRight(0.f)
	,fWaitTime(0.f)

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
	CImageLoader* il = CImageLoader::GetInstance();
	exitButtonData.fileName = "Image\\Buttons\\quit.png";
	exitButtonData.textureID = il->LoadTextureGetID(exitButtonData.fileName.c_str(), false);

	menuButtonData.fileName = "Image\\Buttons\\menu.png";
	menuButtonData.textureID = il->LoadTextureGetID(menuButtonData.fileName.c_str(), false);

	fWaitTime = 0.f;
	fUp = 0.f;
	fRight = 0.f;
	fLeft = 0.f;
	fDown = 0.f;
	bExit = false;

	CSoundController::GetInstance()->PlaySoundByID(SOUND_ID::BGM_WIN);
	return true;
}

bool GameWinState::Update(const double dElapsedTime)
{
	fWaitTime += 0.1f;
	//cout << "down : " << fDown << endl;
	if (cSettings->GetWindowSize().x == 800 && cSettings->GetWindowSize().y == 600)
	{
		if (fWaitTime >= 35.f)
			fUp += 1.f;

		if (fUp == 1100.f)
		{
			fUp -= 1.f; //cap
			fLeft -= 1.f;
			fRight += 1.f;
			fDown += 0.8f;
			if (fLeft <= 400 && fRight >= 400.f)
			{
				fLeft += 1.f;
				fRight -= 1.f;
				fDown -= 0.8f;
			}
		}
	}
	else if (cSettings->GetWindowSize().x == 1024 && cSettings->GetWindowSize().y == 768)
	{
		if (fWaitTime >= 35.f)
			fUp += 1.f;

		if (fUp == 1400.f)
		{
			fUp -= 1.f; //cap
			fLeft -= 1.f;
			fRight += 1.f;
			fDown += 0.8f;
			if (fLeft <= 400 && fRight >= 400.f)
			{
				fLeft += 1.f;
				fRight -= 1.f;
				fDown -= 0.8f;
			}
		}
	}
	else if (cSettings->GetWindowSize().x == 1400 && cSettings->GetWindowSize().y == 1050)
	{
		if (fWaitTime >= 35.f)
			fUp += 1.f;

		if (fUp == 1950.f)
		{
			fUp -= 1.f; //cap
			fLeft -= 1.f;
			fRight += 1.f;
			fDown += 0.8f;
			if (fLeft <= 400 && fRight >= 400.f)
			{
				fLeft += 1.f;
				fRight -= 1.f;
				fDown -= 0.8f;
			}
		}
	}
	else if(cSettings->GetWindowSize().x == 1600)
	{
		if (fWaitTime >= 35.f)
			fUp += 1.f;

		if (fUp == 2000.f)
		{
			fUp -= 1.f; //cap
			fLeft -= 1.f;
			fRight += 1.f;
			fDown += 0.8f;
			if (fLeft <= 400 && fRight >= 400.f)
			{
				fLeft += 1.f;
				fRight -= 1.f;
				fDown -= 0.8f;
			}
		}
	}

	if (bExit)
		return false;

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



	// text window
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
		ImGui::Text("Jevon Liao (191126T)");
		ImGui::Text("");
		ImGui::Text("");
		ImGui::Text("Joash Foo (200528H)"); 
		ImGui::Text("");
		ImGui::Text("");
		ImGui::Text("Sean Lee (200372B)");
		ImGui::SetWindowFontScale(relativeScale_x + relativeScale_y);
		ImGui::PopStyleColor();
		ImGui::End();
		
		ImGui::Begin("Thanks for playing window", NULL, window_flags);
		ImGui::SetWindowSize(ImVec2(1200.0f * relativeScale_x, 1200.0f * relativeScale_y));
		ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.37f - 55, (cSettings->iWindowHeight * 0.01) - 150 + fDown));
		cout << cSettings->iWindowHeight << endl;
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.0f, 1.0f, 1.0f));
		ImGui::Text("Thanks for playing!");
		ImGui::SetWindowFontScale(relativeScale_x + relativeScale_y);
		ImGui::PopStyleColor();
		ImGui::End();
	}

	//menu button window
	{
		ImGui::Begin("Menu Button", NULL, window_flags);
		ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth * .15f - buttonWidth * 0.5f - 400+ fRight,
			CSettings::GetInstance()->iWindowHeight / 2.6f));				// Set the top-left of the window at (10,10)
		ImGui::SetWindowSize(ImVec2((float)CSettings::GetInstance()->iWindowWidth, (float)CSettings::GetInstance()->iWindowHeight));

		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameRounding = 200.0f;
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));

		//back to menu
		if (ImGui::ImageButton((ImTextureID)menuButtonData.textureID,
			ImVec2((relativeScale_x + buttonWidth), (relativeScale_y + buttonHeight)), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
		{
			CSoundController::GetInstance()->StopPlayBack();
			CSoundController::GetInstance()->PlaySoundByID(SOUND_ID::SOUND_ONCLICK);
			CGameStateManager::GetInstance()->SetActiveGameState("MenuState");
		}
		ImGui::PopStyleColor(3);
		ImGui::End();
	}
	//quit button
	{
		ImGui::Begin("Quit Button", NULL, window_flags);
		ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth * 0.84f - buttonWidth * 0.5f + 400 + fLeft,
			CSettings::GetInstance()->iWindowHeight / 2.6f));				// Set the top-left of the window at (10,10)
		ImGui::SetWindowSize(ImVec2((float)CSettings::GetInstance()->iWindowWidth, (float)CSettings::GetInstance()->iWindowHeight));

		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameRounding = 200.0f;
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));

		//exit
		if (ImGui::ImageButton((ImTextureID)exitButtonData.textureID,
			ImVec2((relativeScale_x + buttonWidth), (relativeScale_y + buttonHeight)), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
		{
			CSoundController::GetInstance()->PlaySoundByID(SOUND_ID::SOUND_ONCLICK);
			bExit = true;

		}
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	return true;
}

