#pragma once
#include "../DesignPatterns/SingletonTemplate.h"

#ifndef IMGUI_ACTIVE
#include "..\GUI\imgui.h"
#include "..\GUI\backends\imgui_impl_glfw.h"
#include "..\GUI\backends\imgui_impl_opengl3.h"
#define IMGUI_ACTIVE
#endif


struct sImGuiWindowProperties
{
	bool MoveFromTitleBarOnly = true;
	bool IsDockingEnabled = false;
	bool IsViewportEnabled = false;
};

class CImGuiWindow : public CSingletonTemplate<CImGuiWindow>
{
public:
	CImGuiWindow();
	~CImGuiWindow();

	void Create(const sImGuiWindowProperties& props);
	void Shutdown();

	bool WantCaptureMouse();
	bool WantCaptureKeyboard();

	void Update(const sImGuiWindowProperties& props);

	void PreRender(const sImGuiWindowProperties& props);
	void PostRender();
};

