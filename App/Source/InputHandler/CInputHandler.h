#pragma once
#include "DesignPatterns/SingletonTemplate.h"
#include "Inputs/KeyboardController.h"
#include "Inputs/MouseController.h"

#include "Primitives/Camera2D.h"
#include <vector>
#include <functional>

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
	ARROW_UP,
	ARROW_DOWN,
	ARROW_RIGHT,
	ARROW_LEFT,
	ENTER,
	E,
	R,
	KEY_TOTAL,
};

enum MOUSE_INPUTS
{
	LMB = 0,
	RMB,
	MMB,
	SCROLL,
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
	double dScrollOffset = 0.f;

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
	const static int MAX_STORAGE = 4096;
	const static int RESET_POINT = MAX_STORAGE * 0.5f;

	void Init();
	void Reset(void);
	void Update(double dElapsedTime);

	KeyInput GetKeyInput(int iCurrentFrame, KEYBOARD_INPUTS eInput);
	MouseInput GetMouseInput(int iCurrentFrame, MOUSE_INPUTS eInput);
	std::vector<std::array<KeyInput, KEY_TOTAL>> GetAllKeyboardInputs(void);
	std::vector<std::array<MouseInput, MOUSE_TOTAL>> GetAllMouseInputs(void);

	void SetCallback(std::function<void(int)> ResetFrameStorageCallback) { m_ResetFrameStorageCallback = ResetFrameStorageCallback; }
protected:

	uint32_t m_Keys[KEY_TOTAL] = { GLFW_KEY_W,
								   GLFW_KEY_A,
								   GLFW_KEY_S,
								   GLFW_KEY_D,
								   GLFW_KEY_SPACE,
								   GLFW_KEY_UP,
								   GLFW_KEY_DOWN,
								   GLFW_KEY_LEFT,
								   GLFW_KEY_RIGHT,
								   GLFW_KEY_ENTER,
								   GLFW_KEY_E,
								   GLFW_KEY_R };

	uint32_t m_Buttons[MOUSE_TOTAL] = { CMouseController::LMB,
										CMouseController::RMB, 
										CMouseController::MMB, };

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

	
	std::function<void(int)> m_ResetFrameStorageCallback;

	CInputHandler();
	~CInputHandler();

	void UpdateCurrentFrameInputs(void);
};

