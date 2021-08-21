#pragma once
#include "DesignPatterns/SingletonTemplate.h"
#include "Inputs/KeyboardController.h"
#include "Inputs/MouseController.h"
#include "../Scene2D/Camera2D.h"
#include <vector>

// include glm
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

enum KEYBOARD_INPUTS
{
	W = 0,
	A,
	S,
	D,
	SPACE,
	KEY_TOTAL,
};

enum MOUSE_INPUTS
{
	LMB = 0,
	RMB,
	MMB,
	MOUSE_TOTAL,
};

struct KeyInput
{
	uint32_t Key;
	bool bKeyDown = false;
	bool bKeyPressed = false;
	bool bKeyReleased = false;
	bool bKeyUp = false;
};

struct MouseInput
{
	uint32_t Button;
	bool bButtonDown = false;
	bool bButtonPressed = false;
	bool bButtonReleased = false;
	bool bButtonUp = false;

	glm::vec2 vMousePos = { 0, 0 };
};

class CInputHandler : public CSingletonTemplate<CInputHandler>
{
	friend CSingletonTemplate<CInputHandler>;
public:

	void Init(void);
	void Update(double dElapsedTime);

	KeyInput GetKeyInput(int iCurrentFrame, KEYBOARD_INPUTS eInput);
	MouseInput GetMouseInput(int iCurrentFrame, MOUSE_INPUTS eInput);
	std::vector<std::array<KeyInput, KEY_TOTAL>> GetAllKeyboardInputs(void);
	std::vector<std::array<MouseInput, MOUSE_TOTAL>> GetAllMouseInputs(void);

protected:

	uint32_t m_Keys[KEY_TOTAL] = { GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_SPACE };
	uint32_t m_Buttons[MOUSE_TOTAL] = { CMouseController::LMB, CMouseController::RMB, CMouseController::MMB };

	// Keyboard Inputs stored in a dynamic vector of arrays
	std::vector<std::array<KeyInput, KEY_TOTAL>> m_KeyboardInputs;

	// Mouse Input stored in a dynamic vector of arrays
	std::vector<std::array<MouseInput, MOUSE_TOTAL>> m_MouseInputs;

	// Instance of KeyboardController
	CKeyboardController* cKeyboardController;

	// Instance of MouseController
	CMouseController* cMouseController;

	// Camera Instance
	Camera2D* camera2D;

	CInputHandler();
	~CInputHandler();

	void UpdateCurrentFrameInputs(void);
};

