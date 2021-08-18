#pragma once

/**
 CLevelEditorState
 @brief This class is derived from CGameState. It will introduce the game to the player.
 By: Toh Da Jun
 Date: July 2021
 */

#include "GameStateBase.h"
#include "../KeyboardInputHandler/CKeyboardInputHandler.h"
#include "../LevelEditor/LevelEditor.h"

class CLevelEditorState : public CGameStateBase
{
public:
	// Constructor
	CLevelEditorState(void);
	// Destructor
	~CLevelEditorState(void);

	// Init this class instance
	virtual bool Init(void);
	// Update this class instance
	virtual bool Update(const double dElapsedTime);
	// Render this class instance
	virtual void Render(void);
	// Destroy this class instance
	virtual void Destroy(void);

	virtual bool CreateIMGUI();
	virtual bool DeleteIMGUI();
protected:
	CLevelEditor* cLevelEditor;

	// The handler to the CScene2D instance
	CKeyboardInputHandler* cKeyboardInputHandler;
};
