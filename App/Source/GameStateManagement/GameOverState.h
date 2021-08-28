#pragma once

#include "GameStateBase.h"

#include "Primitives/Mesh.h"
#include "../Scene2D/BackgroundEntity.h"
#include "Math/MyMath.h"

#include <string>

// Include IMGUI
// Important: GLEW and GLFW must be included before IMGUI
#ifndef IMGUI_ACTIVE
#include "GUI\imgui.h"
#include "GUI\backends\imgui_impl_glfw.h"
#include "GUI\backends\imgui_impl_opengl3.h"
#define IMGUI_ACTIVE
#endif


class GameOverState : public CGameStateBase
{
public:
	// Constructor
	GameOverState(void);
	// Destructor
	~GameOverState(void);

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
	CBackgroundEntity* background;
	CSettings* cSettings;
	ButtonData exitButtonData;
	ButtonData gameOverImage;

	float fFadeCount;
	bool bDecrease;

	float fFadeCount2;
	bool bIncrease2;
	bool bDecrease2;

	float fWaitTime;
	bool bWaitTimeIncrease;

};
