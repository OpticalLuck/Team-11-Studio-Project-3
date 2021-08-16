#pragma once
#include "DesignPatterns/SingletonTemplate.h"
#include "Inputs/KeyboardController.h"
#include <vector>

enum KEYBOARD_INPUTS
{
	W = 0,
	A,
	S,
	D,
	SPACE,
	INPUT_TOTAL,
};

class CKeyboardInputHandler : public CSingletonTemplate<CKeyboardInputHandler>
{
	friend CSingletonTemplate<CKeyboardInputHandler>;
public:

	void Init(void);
	void Update(double dElapsedTime);

	bool GetKeyInput(int iCurrentFrame, KEYBOARD_INPUTS eInput);
	std::vector<std::array<bool, INPUT_TOTAL>> GetAllInputs(void);

protected:

	// Keyboard Inputs stored in a dynamic vector of arrays
	std::vector<std::array<bool, INPUT_TOTAL>> m_KeyboardInputs;

	// Instance of KeyboardController
	CKeyboardController* cKeyboardController;

	CKeyboardInputHandler();
	~CKeyboardInputHandler();

	void UpdateCurrentFrameInputs(void);
};

