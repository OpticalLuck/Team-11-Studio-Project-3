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
#include "../LevelEditor/LevelEditor.h"
#include "../GameStateManagement/PlayGameState.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CMenuState::CMenuState(void)
	: background(NULL)
	, menuState(STATE_MAIN)
	, pendingChange(false)
	,cSoundController(NULL)
{

}

/**
 @brief Destructor
 */
CMenuState::~CMenuState(void)
{
	if (background)
	{
		delete background;
		background = nullptr;
	}
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
	background = new CBackgroundEntity("Image/Backgrounds/space.png");
	background->SetShader("2DShader");
	background->Init();

	menuState = STATE_MAIN;

	// Load the images for buttons
	CImageLoader* il = CImageLoader::GetInstance();
	startButtonData.fileName = "Image\\Buttons\\play.png";
	startButtonData.textureID = il->LoadTextureGetID(startButtonData.fileName.c_str(), false);
	exitButtonData.fileName = "Image\\Buttons\\quit.png";
	exitButtonData.textureID = il->LoadTextureGetID(exitButtonData.fileName.c_str(), false);
	editorButtonData.fileName = "Image\\Buttons\\editor.png";
	editorButtonData.textureID = il->LoadTextureGetID(editorButtonData.fileName.c_str(), false);

	optionButtonData.fileName = "Image\\Buttons\\option.png";
	optionButtonData.textureID = il->LoadTextureGetID(optionButtonData.fileName.c_str(), false);
	backButtonData.fileName = "Image\\Buttons\\back.png";
	backButtonData.textureID = il->LoadTextureGetID(backButtonData.fileName.c_str(), false);
	applyButtonData.fileName = "Image\\Buttons\\apply.png";
	applyButtonData.textureID = il->LoadTextureGetID(applyButtonData.fileName.c_str(), false);
	
	cSoundController = CSoundController::GetInstance();

	return true;
}

/**
 @brief Update this class instance
 */
bool CMenuState::Update(const double dElapsedTime)
{
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

	cout << "CMenuState::Destroy()\n" << endl;
}

bool CMenuState::UpdateMenu(ImGuiWindowFlags window_flags)
{
	float buttonWidth = CSettings::GetInstance()->GetWindowSize().x / 5.2;
	float buttonHeight = CSettings::GetInstance()->GetWindowSize().x / (2 * 5.2);

	// Create a window called "Hello, world!" and append into it.
	ImGui::Begin("Main Menu", NULL, window_flags);
	ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 2.0f - buttonWidth / 2.0f, CSettings::GetInstance()->iWindowHeight / 3.0f));				
	ImGui::SetWindowSize(ImVec2(buttonWidth + 55, buttonHeight * 3 + 200));

	//Added rounding for nicer effect
	ImGuiStyle& style = ImGui::GetStyle();
	style.FrameRounding = 200.0f;

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));

	//Start button here
	if (ImGui::ImageButton((ImTextureID)startButtonData.textureID,
		ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
	{
		cSoundController->PlaySoundByID(SOUND_ID::SOUND_ONCLICK);
		menuState = STATE_SELECT_LEVEL;
	}
	//Editor button here
	if (ImGui::ImageButton((ImTextureID)editorButtonData.textureID,
		ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
	{
		cSoundController->PlaySoundByID(SOUND_ID::SOUND_ONCLICK);
		CGameStateManager::GetInstance()->SetActiveGameState("EditorSettingsState");
	}
	//Option button here
	if (ImGui::ImageButton((ImTextureID)optionButtonData.textureID,
		ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
	{
		cSoundController->PlaySoundByID(SOUND_ID::SOUND_ONCLICK);
		menuState = STATE_OPTION;
	}
	//Exit button here
	if (ImGui::ImageButton((ImTextureID)exitButtonData.textureID,
		ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
	{
		cSoundController->PlaySoundByID(SOUND_ID::SOUND_ONCLICK);
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "Quitting the game from MenuState" << endl;

		return false;
	}
	ImGui::PopStyleColor(3);
	ImGui::End();
	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_F1))
	{
		CKeyboardController::GetInstance()->Reset();
		CGameStateManager::GetInstance()->SetActiveGameState("GameOverState");
		cout << "Loading GameOverState state\n";
	}
	//For keyboard controls
	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_SPACE))
	{
		menuState = STATE_SELECT_LEVEL;
	}
	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_E))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "Loading EditorSettingsState" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("EditorSettingsState");
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
		cSoundController->PlaySoundByID(SOUND_ID::SOUND_ONCLICK);
		if (CSettings::GetInstance()->bSFX_Sound == true)
			CSettings::GetInstance()->SFX_VOLUME = sfxVolume;

		CSettings::GetInstance()->UpdateBGMVolume(sfxVolume);
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

bool CMenuState::UpdateLevelSelect(ImGuiWindowFlags window_flags)
{
	//Added rounding for nicer effect
	ImGuiStyle& style = ImGui::GetStyle();
	style.FrameRounding = 100.0f;

	ImGui::Begin("Level Select", NULL, window_flags);
	ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth * 0.25f, CSettings::GetInstance()->iWindowHeight * 0.25f));
	ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth * 0.5f, CSettings::GetInstance()->iWindowHeight * 0.5f));

	if (ImGui::BeginChild("Levels"))
	{
		CLevelEditor* cLevelEditor = CLevelEditor::GetInstance();
		for (unsigned i = 0; i < cLevelEditor->GetLevels().size(); ++i)
		{
			if (ImGui::Button(cLevelEditor->GetLevels()[i].LevelName.c_str()))
			{
				// cLevelEditor->LoadLevel(cLevelEditor->GetLevels()[i].LevelPath.c_str());
				// Reset the CKeyboardController
				CKeyboardController::GetInstance()->Reset();

				// Load the menu state
				cout << "Loading Game" << endl;
				CGameStateManager::GetInstance()->SetActiveGameState("PlayGameState");
				((CPlayGameState*)(CGameStateManager::GetInstance()->GetState("PlayGameState")))->SetLevel(cLevelEditor->GetLevels()[i].LevelPath.c_str());
			}
		}

		ImGui::EndChild();
	}
	ImGui::End();

	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE))
	{
		CKeyboardController::GetInstance()->Reset();
		menuState = STATE_MAIN;
	}

	return true;
}

bool CMenuState::ImGuiRender()
{

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
	case STATE_SELECT_LEVEL:
		window_flags =
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse;

		UpdateLevelSelect(window_flags);
		break;
	}

	return true;
}