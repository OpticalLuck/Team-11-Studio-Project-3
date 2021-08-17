#include "CKeyboardInputHandler.h"
#include <array>
#include <GLFW/glfw3.h>

void CKeyboardInputHandler::Init(void)
{
	cKeyboardController = CKeyboardController::GetInstance();
}

/**
 @brief Only update this Instance once to avoid conflicts
*/
void CKeyboardInputHandler::Update(double dElapsedTime)
{
	UpdateCurrentFrameInputs();
}

/**
 @brief Updates the current frame's inputs to be stored in the input vector
 */
void CKeyboardInputHandler::UpdateCurrentFrameInputs(void)
{
	std::array<bool, INPUT_TOTAL> currentFrameInputs;
	for (int i = 0; i < INPUT_TOTAL; ++i)
	{
		currentFrameInputs[i] = false;
	}

	if (cKeyboardController->IsKeyDown(GLFW_KEY_W))
		currentFrameInputs[W] = true;

	if (cKeyboardController->IsKeyDown(GLFW_KEY_A))
		currentFrameInputs[A] = true;

	if (cKeyboardController->IsKeyDown(GLFW_KEY_S))
		currentFrameInputs[S] = true;

	if (cKeyboardController->IsKeyDown(GLFW_KEY_D))
		currentFrameInputs[D] = true;

	if (cKeyboardController->IsKeyDown(GLFW_KEY_SPACE))
		currentFrameInputs[SPACE] = true;

	m_KeyboardInputs.push_back(currentFrameInputs);
}

bool CKeyboardInputHandler::GetKeyInput(int iCurrentFrame, KEYBOARD_INPUTS eInput)
{
	return m_KeyboardInputs[iCurrentFrame][eInput];
}

std::vector<std::array<bool, INPUT_TOTAL>> CKeyboardInputHandler::GetAllInputs(void)
{
	return m_KeyboardInputs;
}

CKeyboardInputHandler::CKeyboardInputHandler()
	: cKeyboardController(NULL)
{
}

CKeyboardInputHandler::~CKeyboardInputHandler()
{
	cKeyboardController = NULL;
}
