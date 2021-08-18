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

#include "EditorSettingsState.h"

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
CEditorSettingsState::CEditorSettingsState(void)
	: background(NULL)
{

}

/**
 @brief Destructor
 */
CEditorSettingsState::~CEditorSettingsState(void)
{

}

/**
 @brief Init this class instance
 */
bool CEditorSettingsState::Init(void)
{
	cout << "CEditorSettingsState::Init()\n" << endl;

	CShaderManager::GetInstance()->Use("2DShader");
	CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);

	cLevelEditor = CLevelEditor::GetInstance();
	cLevelEditor->SetShader("2DShader");
	cLevelEditor->Init();

	//Create Background Entity
	background = new CBackgroundEntity("Image/MenuBackground.png");
	background->SetShader("2DShader");
	background->Init();

	CreateIMGUI();

	// Load the images for buttons
	CImageLoader* il = CImageLoader::GetInstance();
	
	iWorldWidth = 32;
	iWorldHeight = 24;

	return true;
}

/**
 @brief Update this class instance
 */
bool CEditorSettingsState::Update(const double dElapsedTime)
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

	ImGuiWindowFlags OptionsFlags =
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse;

	//Added rounding for nicer effect
	ImGuiStyle& style = ImGui::GetStyle();
	style.FrameRounding = 100.0f;

	ImGui::Begin("Level Editor", NULL, OptionsFlags);
	ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth * 0.25, CSettings::GetInstance()->iWindowHeight * 0.25));
	ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth * 0.5, CSettings::GetInstance()->iWindowHeight * 0.5));
	
	ImGui::BeginTabBar("Level_Editor#Tab_Bar");
	if (ImGui::BeginTabItem("Select Level"))
	{
		ImGui::BeginChild("Levels");

		for (int i = 0; i < cLevelEditor->GetLevels().size(); ++i)
		{
			if (ImGui::Button(cLevelEditor->GetLevels()[i].LevelName.c_str()))
			{
				cLevelEditor->LoadLevel(cLevelEditor->GetLevels()[i].LevelPath.c_str());
				// Reset the CKeyboardController
				CKeyboardController::GetInstance()->Reset();

				// Load the menu state
				cout << "Loading LevelEditorState" << endl;
				CGameStateManager::GetInstance()->SetActiveGameState("LevelEditorState");
			}
		}

		ImGui::EndChild();

		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Create Level"))
	{
		ImGui::InputInt("Level Width", &iWorldWidth);
		ImGui::InputInt("Level Height", &iWorldHeight);

		if (iWorldWidth < 32 || iWorldHeight < 24)
		{
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Please input a reasonable Level size");
		}
		else if (ImGui::Button("Create Level"))
		{
			cLevelEditor->CreateLevel("", iWorldWidth, iWorldHeight);
		}


		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
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

/**
 @brief Render this class instance
 */
void CEditorSettingsState::Render(void)
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
void CEditorSettingsState::Destroy(void)
{
	// Delete the background
	if (background)
	{
		delete background;
		background = NULL;
	}

	// Cleanup
	DeleteIMGUI();

	cout << "CEditorSettingsState::Destroy()\n" << endl;
}

bool CEditorSettingsState::CreateIMGUI()
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

bool CEditorSettingsState::DeleteIMGUI()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	return true;
}

