#pragma once
#include "GameStateBase.h"

#include "Primitives/Mesh.h"
#include "../Scene2D/BackgroundEntity.h"

#include <string>

// Include IMGUI
// Important: GLEW and GLFW must be included before IMGUI
#ifndef IMGUI_ACTIVE
#include "GUI\imgui.h"
#include "GUI\backends\imgui_impl_glfw.h"
#include "GUI\backends\imgui_impl_opengl3.h"
#define IMGUI_ACTIVE
#endif
class PauseMenuState : public CGameStateBase
{
public:
	// Constructor
	PauseMenuState(void);
	// Destructor
	~PauseMenuState(void);

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
	enum MENUSTATE
	{
		STATE_MAIN = 0,
		STATE_OPTION,
		STATE_TOTAL
	};
	MENUSTATE menuState;

	CBackgroundEntity* background;
	ButtonData resumeButtonData;
	ButtonData exitButtonData;

	ButtonData optionButtonData;
	ButtonData backButtonData;
	ButtonData applyButtonData;

	bool pendingChange;

	bool UpdateMenu(ImGuiWindowFlags window_flags);
	void UpdateOption(ImGuiWindowFlags window_flags);
};

