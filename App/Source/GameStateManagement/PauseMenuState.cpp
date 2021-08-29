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

#include "PauseMenuState.h"

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
#include <iostream>

using namespace std;

PauseMenuState::PauseMenuState(void)
	: background(NULL)
	, menuState(STATE_MAIN)
	, pendingChange(false)
	, cSoundController(NULL)
{
}

PauseMenuState::~PauseMenuState(void)
{
}

bool PauseMenuState::Init(void)
{
	cout << "CPauseState::Init()\n" << endl;


	// Load the images for buttons
	CImageLoader* il = CImageLoader::GetInstance();
	resumeButtonData.fileName = "Image\\Buttons\\play.png";
	resumeButtonData.textureID = il->LoadTextureGetID(resumeButtonData.fileName.c_str(), false);
	exitButtonData.fileName = "Image\\Buttons\\quit.png";
	exitButtonData.textureID = il->LoadTextureGetID(exitButtonData.fileName.c_str(), false);

	optionButtonData.fileName = "Image\\Buttons\\option.png";
	optionButtonData.textureID = il->LoadTextureGetID(optionButtonData.fileName.c_str(), false);
	backButtonData.fileName = "Image\\Buttons\\back.png";
	backButtonData.textureID = il->LoadTextureGetID(backButtonData.fileName.c_str(), false);
	applyButtonData.fileName = "Image\\Buttons\\apply.png";
	applyButtonData.textureID = il->LoadTextureGetID(applyButtonData.fileName.c_str(), false);

	cSoundController = CSoundController::GetInstance();

	return true;

}

bool PauseMenuState::Update(const double dElapsedTime)
{
	return true;
}

void PauseMenuState::Render(void)
{
	// Clear the screen and buffer
	glClearColor(0.0f, 0.55f, 1.00f, 1.00f);

}

void PauseMenuState::Destroy(void)
{
	cout << "CMenuState::Destroy()\n" << endl;
}

bool PauseMenuState::UpdateMenu(ImGuiWindowFlags window_flags)
{
	option_window = 0;
	option_window |= ImGuiWindowFlags_NoTitleBar;
	option_window |= ImGuiWindowFlags_NoScrollbar;
	//window_flags |= ImGuiWindowFlags_MenuBar;
	option_window |= ImGuiWindowFlags_NoBackground;
	option_window |= ImGuiWindowFlags_NoMove;
	option_window |= ImGuiWindowFlags_NoCollapse;
	option_window |= ImGuiWindowFlags_NoNav;

	float buttonWidth = CSettings::GetInstance()->GetWindowSize().x / 5.2;
	float buttonHeight = CSettings::GetInstance()->GetWindowSize().x / (2 * 5.2);
	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		// Create a window called "Hello, world!" and append into it.
		ImGui::Begin("Option", NULL, window_flags);
		ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 2.0f - buttonWidth / 2.0f,
			CSettings::GetInstance()->iWindowHeight / 3.0f));				// Set the top-left of the window at (10,10)
		ImGui::SetWindowSize(ImVec2((float)CSettings::GetInstance()->iWindowWidth, (float)CSettings::GetInstance()->iWindowHeight));


		//Added rounding for nicer effect
		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameRounding = 200.0f;
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));


		// Play button
		if (ImGui::ImageButton((ImTextureID)resumeButtonData.textureID,
			ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
		{
			cSoundController->PlaySoundByID(SOUND_ID::SOUND_ONCLICK);
			// Reset the CKeyboardController
			CKeyboardController::GetInstance()->Reset();

			// Load the menu state
			cout << "Returning to PlayGameState" << endl;
			CGameStateManager::GetInstance()->OffPauseGameState();
		}
		if (ImGui::ImageButton((ImTextureID)optionButtonData.textureID,
			ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
		{
			cSoundController->PlaySoundByID(SOUND_ID::SOUND_ONCLICK);
			menuState = STATE_OPTION;
		}
		//end button
		if (ImGui::ImageButton((ImTextureID)exitButtonData.textureID,
			ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
		{
			cSoundController->PlaySoundByID(SOUND_ID::SOUND_ONCLICK);
			// Reset the CKeyboardController
			CKeyboardController::GetInstance()->Reset();

			CGameStateManager::GetInstance()->OffPauseGameState();
			CGameStateManager::GetInstance()->SetActiveGameState("MenuState");
		}
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	return true;
}

void PauseMenuState::UpdateOption(ImGuiWindowFlags window_flags)
{
	float buttonWidth = 256;
	float buttonHeight = 128;

	ImGui::Begin("Options", NULL, window_flags);

	ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetWindowSize(ImVec2((float)CSettings::GetInstance()->iWindowWidth, (float)CSettings::GetInstance()->iWindowHeight - buttonHeight - 50.f));

	//Resolution
	static int screenRes = (int)CSettings::GetInstance()->screenSize;
	const char* resNames[(int)CSettings::SCREENSIZE::SSIZE_TOTAL] = { "800 x 600", "1024 x 768", "1400 x 1050"};
	const char* currName = (screenRes >= 0 && screenRes < (int)CSettings::SCREENSIZE::SSIZE_TOTAL) ? resNames[screenRes] : "Unknown";
	if (ImGui::SliderInt("Screen Resolution", &screenRes, 0, (int)CSettings::SCREENSIZE::SSIZE_TOTAL - 1, currName, ImGuiSliderFlags_NoInput))
	{
		pendingChange = true;
	}

	ImGui::NewLine();
	ImGui::Text("Audio");

	//Master Volume
	static float master_Volume = CSettings::GetInstance()->MASTER_VOLUME;
	if (ImGui::SliderFloat("Master Volume", &master_Volume, 0.f, 100.f, "%.1f"))
	{
		CSettings::GetInstance()->MASTER_VOLUME = master_Volume;
		CSettings::GetInstance()->UpdateMasterVolume();
	}

	static float bgmVolume = CSettings::GetInstance()->BGM_VOLUME;
	bool bgmChange = false;

	if (ImGui::Checkbox("Enable BackGround Music", &CSettings::GetInstance()->bBGM_Sound))
	{
		cSoundController->PlaySoundByID(SOUND_ID::SOUND_ONCLICK);
		bgmChange = true;
	}

	if (CSettings::GetInstance()->bBGM_Sound == true)
	{
		bgmVolume = CSettings::GetInstance()->BGM_VOLUME;
	}
	else
	{
		bgmVolume = 0.f;
	}

	if(ImGui::SliderFloat("BGM Volume", &bgmVolume, 0.f, 100.f, "%.2f"))
	{
		bgmChange = true;
		if (bgmVolume > 0.f)
		{
			CSettings::GetInstance()->bBGM_Sound = true;
		}
		else
		{
			CSettings::GetInstance()->bBGM_Sound = false;
			bgmVolume = 0.f;
		}
	}

	if (bgmChange)
	{
		if (CSettings::GetInstance()->bBGM_Sound == true)
			CSettings::GetInstance()->BGM_VOLUME = bgmVolume;

		CSettings::GetInstance()->UpdateBGMVolume(bgmVolume);
	}

	//SFX Volume
	static float sfxVolume = CSettings::GetInstance()->SFX_VOLUME;
	bool sfxChange = false;
	if (ImGui::Checkbox("Enable Sound Effect", &CSettings::GetInstance()->bSFX_Sound))
	{
		cSoundController->PlaySoundByID(SOUND_ID::SOUND_ONCLICK);
		sfxChange = true;
	}

	if (CSettings::GetInstance()->bSFX_Sound == true)
	{
		sfxVolume = CSettings::GetInstance()->SFX_VOLUME;
	}
	else
	{
		sfxVolume = 0.f;
	}

	if (ImGui::SliderFloat("SFX Volume", &sfxVolume, 0.f, 100.f, "%.2f"))
	{
		sfxChange = true;
		if (sfxVolume > 0.f)
		{
			CSettings::GetInstance()->bSFX_Sound = true;
		}
		else
		{
			CSettings::GetInstance()->bSFX_Sound = false;
			sfxVolume = 0.f;
		}
	}

	ImGui::Checkbox("Show Colliders", &CSettings::GetInstance()->bShowCollider);

	if (sfxChange)
	{
		if (CSettings::GetInstance()->bSFX_Sound == true)
			CSettings::GetInstance()->SFX_VOLUME = sfxVolume;

		CSettings::GetInstance()->UpdateSFXVolume(sfxVolume);
	}

	ImGui::End();

	window_flags |= ImGuiWindowFlags_NoBackground;
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));


	ImGui::Begin("Apply", NULL, window_flags);
	ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth - buttonWidth - 20, CSettings::GetInstance()->iWindowHeight - buttonHeight - 20));
	ImGui::SetWindowSize(ImVec2(buttonWidth + 20, buttonHeight + 20));

	if (ImGui::ImageButton((ImTextureID)applyButtonData.textureID,
		ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
	{
		cSoundController->PlaySoundByID(SOUND_ID::SOUND_ONCLICK);
		if (pendingChange)
		{
			cSoundController->PlaySoundByID(SOUND_ID::SOUND_ONCLICK);
			pendingChange = false;
			CSettings::GetInstance()->screenSize = static_cast<CSettings::SCREENSIZE>(screenRes);
			CSettings::GetInstance()->UpdateWindowSize();
		}
	}
	ImGui::End();

	ImGui::Begin("Back", NULL, window_flags);
	ImGui::SetWindowPos(ImVec2(0, CSettings::GetInstance()->iWindowHeight - buttonHeight - 20));
	ImGui::SetWindowSize(ImVec2(buttonWidth + 20, buttonHeight + 20));

	if (ImGui::ImageButton((ImTextureID)backButtonData.textureID,
		ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
	{
		cSoundController->PlaySoundByID(SOUND_ID::SOUND_ONCLICK);
		menuState = STATE_MAIN;
	}
	ImGui::End();

	ImGui::PopStyleColor(3);

	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE))
	{
		menuState = STATE_MAIN;
	}

	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ENTER))
	{
		if (pendingChange)
		{
			pendingChange = false;
			CSettings::GetInstance()->screenSize = static_cast<CSettings::SCREENSIZE>(screenRes);
			CSettings::GetInstance()->UpdateWindowSize();
		}
	}
}

bool PauseMenuState::ImGuiRender()
{

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	//window_flags |= ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoNav;
	window_flags |= ImGuiWindowFlags_NoResize;

	//UpdateMenu(window_flags);
	switch (menuState)
	{
	case STATE_MAIN:
		window_flags |= ImGuiWindowFlags_NoBackground;
		if (UpdateMenu(window_flags) == false)
		{
			return false;
		}
		break;
	case STATE_OPTION:
		UpdateOption(window_flags);
		break;
	}


	return true;
}

//bool PauseMenuState::OptionImGui()
//{
//	return false;
//}
