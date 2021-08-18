// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <GLFW/glfw3.h>

#include "LevelEditorState.h"

// Include CGameStateManager
#include "GameStateManager.h"

// Include CKeyboardController
#include "Inputs/KeyboardController.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CLevelEditorState::CLevelEditorState(void)
	: cKeyboardInputHandler(NULL)
	, cLevelEditor(NULL)
{

}

/**
 @brief Destructor
 */
CLevelEditorState::~CLevelEditorState(void)
{
	Destroy();
}

/**
 @brief Init this class instance
 */
bool CLevelEditorState::Init(void)
{
	cout << "CLevelEditorState::Init()\n" << endl;

	cLevelEditor = CLevelEditor::GetInstance();

	Camera2D::GetInstance()->Reset();
	Camera2D::GetInstance()->UpdateTarget(glm::vec2(CSettings::GetInstance()->NUM_TILES_XAXIS * 0.5, CSettings::GetInstance()->NUM_TILES_YAXIS * 0.5));

	cKeyboardInputHandler = CKeyboardInputHandler::GetInstance();
	cKeyboardInputHandler->Init();

	return true;
}

/**
 @brief Update this class instance
 */
bool CLevelEditorState::Update(const double dElapsedTime)
{
	Camera2D::GetInstance()->Update(dElapsedTime);

	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "Loading PauseState" << endl;
		CGameStateManager::GetInstance()->SetPauseGameState("PauseState");
		return true;
	}
	if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_W))
	{
		Camera2D::GetInstance()->UpdateTarget(glm::vec2(Camera2D::GetInstance()->getTarget().x, Camera2D::GetInstance()->getTarget().y + 0.2));
	}
	if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_S))
	{
		Camera2D::GetInstance()->UpdateTarget(glm::vec2(Camera2D::GetInstance()->getTarget().x, Camera2D::GetInstance()->getTarget().y - 0.2));
	}
	if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_A))
	{
		Camera2D::GetInstance()->UpdateTarget(glm::vec2(Camera2D::GetInstance()->getTarget().x - 0.2, Camera2D::GetInstance()->getTarget().y));
	}
	if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_D))
	{
		Camera2D::GetInstance()->UpdateTarget(glm::vec2(Camera2D::GetInstance()->getTarget().x + 0.2, Camera2D::GetInstance()->getTarget().y));
	}

	return true;
}

/**
 @brief Render this class instance
 */
void CLevelEditorState::Render(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(1.00f, 1.00f, 1.00f, 1.00f);

	cLevelEditor->PreRender();
	cLevelEditor->Render();
	cLevelEditor->PostRender();
}

/**
 @brief Destroy this class instance
 */
void CLevelEditorState::Destroy(void)
{
	cout << "CLevelEditorState::Destroy()\n" << endl;
}

bool CLevelEditorState::CreateIMGUI()
{
	return false;
}

bool CLevelEditorState::DeleteIMGUI()
{
	return false;
}
