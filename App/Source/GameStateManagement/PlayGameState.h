#pragma once

/**
 CPlayGameState
 @brief This class is derived from CGameState. It will introduce the game to the player.
 By: Toh Da Jun
 Date: July 2021
 */

#include "GameStateBase.h"
#include "../InputHandler/CInputHandler.h"
#include "TimeControl/FPSCounter.h"
#include "..\Scene2D\Scene2D.h"

class CPlayGameState : public CGameStateBase
{
public:
	// Constructor
	CPlayGameState(void);
	// Destructor
	~CPlayGameState(void);

	// Init this class instance
	virtual bool Init(void);
	// Update this class instance
	virtual bool Update(const double dElapsedTime);
	// Render this class instance
	virtual void Render(void);
	// Destroy this class instance
	virtual void Destroy(void);

	virtual bool ImGuiRender();
protected:
	// The handler to the CScene2D instance
	CScene2D* cScene2D;
	CInputHandler* cInputHandler;

	// FPS Control
	CFPSCounter* cFPSCounter;

	// Flags for IMGUI
	ImGuiWindowFlags window_flags;
	float m_fProgressBar;

	float fInterval;
	int iMinutes;
	int iSeconds;

	CSettings* cSettings;
	CTextureManager* cTextureManager;
	CInventory* cPlayerInventory;

	CKeyboardController* cKeyboardController;
};
