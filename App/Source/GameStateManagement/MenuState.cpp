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

#include "MenuState.h"

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

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CMenuState::CMenuState(void)
	: background(NULL)
	, menuState(STATE_MAIN)
	, pendingChange(false)
{

}

/**
 @brief Destructor
 */
CMenuState::~CMenuState(void)
{

}

/**
 @brief Init this class instance
 */
bool CMenuState::Init(void)
{
	cout << "CMenuState::Init()\n" << endl;

	CShaderManager::GetInstance()->Use("2DShader");
	CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);

	//Create Background Entity
	background = new CBackgroundEntity("Image/MenuBackground.png");
	background->SetShader("2DShader");
	background->Init();

	CreateIMGUI();

	// Load the images for buttons
	CImageLoader* il = CImageLoader::GetInstance();
	startButtonData.fileName = "Image\\GUI\\PlayButton.png";
	startButtonData.textureID = il->LoadTextureGetID(startButtonData.fileName.c_str(), false);
	exitButtonData.fileName = "Image\\GUI\\ExitButton.png";
	exitButtonData.textureID = il->LoadTextureGetID(exitButtonData.fileName.c_str(), false);

	optionButtonData.fileName = "Image\\GUI\\OptionButton.png";
	optionButtonData.textureID = il->LoadTextureGetID(optionButtonData.fileName.c_str(), false);
	backButtonData.fileName = "Image\\GUI\\BackButton.png";
	backButtonData.textureID = il->LoadTextureGetID(backButtonData.fileName.c_str(), false);
	applyButtonData.fileName = "Image\\GUI\\ApplyButton.png";
	applyButtonData.textureID = il->LoadTextureGetID(applyButtonData.fileName.c_str(), false);

	return true;
}

/**
 @brief Update this class instance
 */
bool CMenuState::Update(const double dElapsedTime)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	//window_flags |= ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoNav;
	window_flags |= ImGuiWindowFlags_NoResize;

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

/**
 @brief Render this class instance
 */
void CMenuState::Render(void)
{
	// Clear the screen and buffer
	glClearColor(0.0f, 0.55f, 1.00f, 1.00f);

	//Render Background
	background->Render();

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/**
 @brief Destroy this class instance
 */
void CMenuState::Destroy(void)
{
	// Delete the background
	if (background)
	{
		delete background;
		background = NULL;
	}

	// Cleanup
	DeleteIMGUI();

	cout << "CMenuState::Destroy()\n" << endl;
}

bool CMenuState::CreateIMGUI()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(CSettings::GetInstance()->pWindow, true);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	return true;
}

bool CMenuState::DeleteIMGUI()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	return true;
}

bool CMenuState::UpdateMenu(ImGuiWindowFlags window_flags)
{
	float buttonWidth = 256;
	float buttonHeight = 128;

	// Create a window called "Hello, world!" and append into it.
	ImGui::Begin("Main Menu", NULL, window_flags);
	ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 2.0f - buttonWidth / 2.0f, CSettings::GetInstance()->iWindowHeight / 6.0f));				
	ImGui::SetWindowSize(ImVec2(buttonWidth + 25, buttonHeight * 3 + 50));

	//Added rounding for nicer effect
	ImGuiStyle& style = ImGui::GetStyle();
	style.FrameRounding = 200.0f;

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));

	// Add codes for Start button here
	if (ImGui::ImageButton((ImTextureID)startButtonData.textureID,
		ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "Loading PlayGameState" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("PlayGameState");
	}

	if (ImGui::ImageButton((ImTextureID)optionButtonData.textureID,
		ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
	{
		menuState = STATE_OPTION;
	}

	// Add codes for Exit button here
	if (ImGui::ImageButton((ImTextureID)exitButtonData.textureID,
		ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "Quitting the game from MenuState" << endl;

		return false;
	}
	ImGui::PopStyleColor(3);
	ImGui::End();

	//For keyboard controls
	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_SPACE))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "Loading PlayGameState" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("PlayGameState");
		return true;
	}
	else if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "Quitting the game from MenuState" << endl;
		return false;
	}

	return true;
}

void CMenuState::UpdateOption(ImGuiWindowFlags window_flags)
{
	float buttonWidth = 256;
	float buttonHeight = 128;

	ImGui::Begin("Options", NULL, window_flags);

	ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetWindowSize(ImVec2((float)CSettings::GetInstance()->iWindowWidth, (float)CSettings::GetInstance()->iWindowHeight - buttonHeight - 50.f));

	//Resolution
	static int screenRes = CSettings::GetInstance()->screenSize;
	const char* resNames[CSettings::SSIZE_TOTAL] = { "800 x 600", "1024 x 768", "1400 x 1050", "1600 x 1200" };
	const char* currName = (screenRes >= 0 && screenRes < CSettings::SSIZE_TOTAL) ? resNames[screenRes] : "Unknown";
	if (ImGui::SliderInt("Screen Resolution", &screenRes, 0, CSettings::SSIZE_TOTAL - 1, currName, ImGuiSliderFlags_NoInput))
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
		CSoundController::GetInstance()->SetMasterVolume(CSettings::GetInstance()->MASTER_VOLUME * 0.01f);
	}

	static float bgmVolume = CSettings::GetInstance()->BGM_VOLUME;
	bool bgmChange = false;

	if (ImGui::Checkbox("Enable BackGround Music", &CSettings::GetInstance()->bBGM_Sound))
	{
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

	if (ImGui::SliderFloat("BGM Volume", &bgmVolume, 0.f, 100.f, "%.2f"))
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

		CSoundController::GetInstance()->SetVolumeByID(4, bgmVolume * 0.01f);
		CSoundController::GetInstance()->UpdatePlayBackVolume(bgmVolume * 0.01f);
	}

	//SFX Volume
	static float sfxVolume = CSettings::GetInstance()->SFX_VOLUME;
	bool sfxChange = false;
	if (ImGui::Checkbox("Enable Sound Effect", &CSettings::GetInstance()->bSFX_Sound))
	{
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

	if (sfxChange)
	{
		if (CSettings::GetInstance()->bSFX_Sound == true)
			CSettings::GetInstance()->SFX_VOLUME = sfxVolume;

		for (int i = 0; i < 10; i++)
		{
			if (i != 4)
			{
				CSoundController::GetInstance()->SetVolumeByID(i, sfxVolume * 0.01f);
			}
		}
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
		if (pendingChange)
		{
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
