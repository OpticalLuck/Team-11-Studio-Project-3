#include "CInputHandler.h"
#include <array>
#include <GLFW/glfw3.h>

void CInputHandler::Init(void)
{
	cKeyboardController = CKeyboardController::GetInstance();
	cMouseController = CMouseController::GetInstance();
	camera2D = Camera2D::GetInstance();
}

/**
 @brief Only update this Instance once to avoid conflicts
*/
void CInputHandler::Update(double dElapsedTime)
{
	UpdateCurrentFrameInputs();
}

/**
 @brief Updates the current frame's inputs to be stored in the input vector
 */
void CInputHandler::UpdateCurrentFrameInputs(void)
{
	std::array<KeyInput, KEY_TOTAL> currentFrameKeyInputs;
	for (int i = 0; i < KEY_TOTAL; ++i)
	{
		currentFrameKeyInputs[i].Key = m_Keys[i];

		if (cKeyboardController->IsKeyDown(m_Keys[i]))
			currentFrameKeyInputs[i].bKeyDown = true;

		if (cKeyboardController->IsKeyPressed(m_Keys[i]))
			currentFrameKeyInputs[i].bKeyPressed = true;

		if (cKeyboardController->IsKeyReleased(m_Keys[i]))
			currentFrameKeyInputs[i].bKeyReleased = true;

		if (cKeyboardController->IsKeyUp(m_Keys[i]))
			currentFrameKeyInputs[i].bKeyUp = true;
	}

	glm::vec2 mousePos = camera2D->GetCursorPosInWorldSpace();
	std::array<MouseInput, MOUSE_TOTAL> currentFrameMouseInputs;
	for (int i = 0; i < MOUSE_TOTAL; ++i)
	{
		currentFrameMouseInputs[i].Button = m_Buttons[i];
		currentFrameMouseInputs[i].vMousePos = mousePos;

		if (cMouseController->IsButtonDown(m_Buttons[i]))
			currentFrameMouseInputs[i].bButtonDown = true;

		if (cMouseController->IsButtonPressed(m_Buttons[i]))
			currentFrameMouseInputs[i].bButtonPressed = true;

		if (cMouseController->IsButtonReleased(m_Buttons[i]))
			currentFrameMouseInputs[i].bButtonReleased = true;

		if (cMouseController->IsButtonUp(m_Buttons[i]))
			currentFrameMouseInputs[i].bButtonUp = true;

		currentFrameMouseInputs[i].dScrollOffset = cMouseController->GetMouseScrollStatus(CMouseController::SCROLL_TYPE_YOFFSET);

	}

	m_KeyboardInputs.push_back(currentFrameKeyInputs);
	m_MouseInputs.push_back(currentFrameMouseInputs);
}

KeyInput CInputHandler::GetKeyInput(int iCurrentFrame, KEYBOARD_INPUTS eInput)
{
	return m_KeyboardInputs[iCurrentFrame][eInput];
}

std::vector<std::array<KeyInput, KEY_TOTAL>> CInputHandler::GetAllKeyboardInputs(void)
{
	return m_KeyboardInputs;
}

MouseInput CInputHandler::GetMouseInput(int iCurrentFrame, MOUSE_INPUTS eInput)
{
	return m_MouseInputs[iCurrentFrame][eInput];
}

std::vector<std::array<MouseInput, MOUSE_TOTAL>> CInputHandler::GetAllMouseInputs(void)
{
	return m_MouseInputs;
}


CInputHandler::CInputHandler()
	: cKeyboardController(NULL)
	, cMouseController(NULL)
	, camera2D(NULL)
{
}

CInputHandler::~CInputHandler()
{
	cKeyboardController = NULL;
	cMouseController = NULL;
	camera2D = NULL;
}

void CInputHandler::Reset() {
	m_KeyboardInputs.clear();
	m_MouseInputs.clear();
}
