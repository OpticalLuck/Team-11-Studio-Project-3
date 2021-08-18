#include "ImGuiWindow.h"
#include "../GameControl/Settings.h"

CImGuiWindow::CImGuiWindow()
{
}

CImGuiWindow::~CImGuiWindow()
{
}

void CImGuiWindow::Create(const sImGuiWindowProperties& props)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigWindowsMoveFromTitleBarOnly = props.MoveFromTitleBarOnly;
	if (props.IsDockingEnabled)
	{
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	}
	if (props.IsViewportEnabled)
	{
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	}

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(CSettings::GetInstance()->pWindow, true);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

}

void CImGuiWindow::Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

bool CImGuiWindow::WantCaptureMouse()
{
	return ImGui::GetIO().WantCaptureMouse;
}

bool CImGuiWindow::WantCaptureKeyboard()
{
	return ImGui::GetIO().WantCaptureKeyboard;
}

void CImGuiWindow::Update(const sImGuiWindowProperties& props)
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigWindowsMoveFromTitleBarOnly = props.MoveFromTitleBarOnly;
	if (props.IsDockingEnabled)
	{
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	}
	if (props.IsViewportEnabled)
	{
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	}
}

void CImGuiWindow::PreRender(const sImGuiWindowProperties& props)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (props.IsDockingEnabled)
	{
		// ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	}
}

void CImGuiWindow::PostRender()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(current_context);
	}
}
