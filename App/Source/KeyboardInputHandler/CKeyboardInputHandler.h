#pragma once
#include "DesignPatterns/SingletonTemplate.h"
#include "Inputs/KeyboardController.h"
#include <vector>

class CKeyboardInputHandler : public CSingletonTemplate<CKeyboardInputHandler>
{
	friend CSingletonTemplate<CKeyboardInputHandler>;
public:

	enum KEYBOARD_INPUTS
	{
		W = 0,
		A,
		S,
		D,
		SPACE,
		INPUT_TOTAL,
	};

	// Keyboard Inputs stored in a dynamic vector of arrays
	std::vector<std::array<bool, INPUT_TOTAL>> mKeyboardInputs;

	void Init(void);
	void Update(double dElapsedTime);
protected:

	// Instance of KeyboardController
	CKeyboardController* cKeyboardController;

	CKeyboardInputHandler();
	~CKeyboardInputHandler();

	void UpdateCurrentFrameInputs(void);
};

