// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <Windows.h>
#include <GLFW/glfw3.h>

#include "LevelEditorState.h"

// Include CGameStateManager
#include "GameStateManager.h"

#include "System/Debug.h"

#include "Math/MyMath.h"

#include "ImGuiWindow/ImGuiWindow.h"

#include "System/WindowUtils.h"

#include "../Scene2D/Object2D.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CLevelEditorState::CLevelEditorState(void)
	: cKeyboardController(NULL)
	, cLevelEditor(NULL)
	, cMouseController(NULL)
	, cSettings(NULL)
	, cLevelGrid(NULL)
	, activeTile(0)
	, transform(1.f)
	, cursor(NULL)
	, FBO(0)
	, RBO(0)
	, quadVAO(0)
	, quadVBO(0)
	, textureColorBuffer(0)
{
	vMousePos = glm::i32vec2();
}
/**
 @brief Destructor
 */
CLevelEditorState::~CLevelEditorState(void)
{
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);

	cKeyboardController = NULL;
	cLevelEditor = NULL;
	cMouseController = NULL;
	cSettings = NULL;

	if (cLevelGrid)
	{ 
		cLevelGrid->Destroy();
		cLevelGrid = NULL;
	}

	Destroy();
}

/**
 @brief Init this class instance
 */
bool CLevelEditorState::Init(void)
{
	cout << "CLevelEditorState::Init()\n" << endl;

	eProperties.Reset();

	cSettings = CSettings::GetInstance();
	// cSettings->screenSize = CSettings::SCREENSIZE::SSIZE_1024x768;
	// cSettings->m_ImGuiWindow->Update(cSettings->ImGuiProperties);
	// CSettings::GetInstance()->UpdateWindowSize();

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);

	// GenerateQuadVAO();
	// GenerateFBO();

	cursor = CMeshBuilder::GenerateQuad();

	cLevelEditor = CLevelEditor::GetInstance();
	cLevelGrid = CLevelGrid::GetInstance();
	cLevelGrid->Init(cLevelEditor->iWorldWidth, cLevelEditor->iWorldHeight);

	ActionState currentState;
	currentState.m_ThisActionLevel = cLevelEditor->GetCurrentLevel();
	currentState.iWorldWidth = cLevelEditor->iWorldWidth;
	currentState.iWorldHeight = cLevelEditor->iWorldHeight;

	eProperties.undoStates.push_back(currentState);
	eProperties.redoStates.clear();

	Camera2D::GetInstance()->Reset();
	Camera2D::GetInstance()->UpdateTarget(glm::vec2(cSettings->NUM_TILES_XAXIS * 0.5, cSettings->NUM_TILES_YAXIS * 0.5));

	cMouseController = CMouseController::GetInstance();
	cKeyboardController = CKeyboardController::GetInstance();

	return true;
}

/**
 @brief Update this class instance
 */
bool CLevelEditorState::Update(const double dElapsedTime)
{
	Camera2D::GetInstance()->Update((float)dElapsedTime);

	vMousePos = Camera2D::GetInstance()->GetCursorPosInWorldSpace(0);

	float mouseX = (float)Math::Clamp((float)vMousePos.x, 0.f, (float)cLevelEditor->iWorldWidth - 1.f);
	float mouseY = (float)Math::Clamp((float)vMousePos.y, 0.f, (float)cLevelEditor->iWorldHeight - 1.f);

	vMousePos = glm::i32vec2(mouseX, mouseY);

	if (cKeyboardController->IsKeyReleased(GLFW_KEY_ESCAPE))
	{
		Close();
	}
	if (cKeyboardController->IsKeyPressed(GLFW_KEY_TAB))
	{
		eProperties.bToggleEditorWindow = !eProperties.bToggleEditorWindow;
	}
	if (cKeyboardController->IsKeyPressed(GLFW_KEY_G))
	{
		eProperties.bShowGrids = !eProperties.bShowGrids;
	}

	if (!CImGuiWindow::GetInstance()->WantCaptureMouse())
	{
		// Disable default mouse events if currently using shortcuts
		if (!EditorShortcuts())
		{
			MouseInput(dElapsedTime);
			ScaleMap();
		}

		// Mouse X Scroll
		if (cMouseController->GetMousePositionX() < 100 || cMouseController->GetMousePositionX() > cSettings->iWindowWidth - 100)
			Camera2D::GetInstance()->MoveTarget(-((float)cSettings->iWindowWidth * 0.5f - (float)cMouseController->GetMousePositionX()) * (float)dElapsedTime * 0.025f, 0);
		
		// Mouse Y Scroll
		if (cMouseController->GetMousePositionY() < 100 || cMouseController->GetMousePositionY() > cSettings->iWindowHeight - 100)
			Camera2D::GetInstance()->MoveTarget(0, ((float)cSettings->iWindowHeight * 0.5f - (float)cMouseController->GetMousePositionY()) * (float)dElapsedTime * 0.025f);
	}

	FileUtilShortcuts();
	MoveCamera(dElapsedTime);

	if (cKeyboardController->IsKeyDown(GLFW_KEY_4))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (cKeyboardController->IsKeyDown(GLFW_KEY_3))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}


	return true;
}

/**
 @brief Render this class instance
 */
void CLevelEditorState::Render(void)
{
	//glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	cLevelEditor->RenderBackground();

	cLevelEditor->PreRender();
	cLevelEditor->Render();
	cLevelEditor->PostRender();

	if (eProperties.bShowGrids)
	{
		cLevelGrid->PreRender();
		cLevelGrid->Render();
		cLevelGrid->PostRender();
	}

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	RenderCursor();
	// ImGuiRender();
}

/**
 @brief Destroy this class instance
 */
void CLevelEditorState::Destroy(void)
{
	cout << "CLevelEditorState::Destroy()\n" << endl;
}


bool CLevelEditorState::EditorShortcuts(void)
{
	// SHIFT+LMB - Area Fill
	// SHIFT+RMB - Area Delete
	// CTRL+Z - Undo
	// CTRL+Y - Redo
	// CTRL+S - Save
	// ESCAPE - Close

	if (cKeyboardController->IsKeyDown(GLFW_KEY_LEFT_SHIFT))
	{
		// DEBUG_MSG("Holding LSHIFT");
		if (cMouseController->IsButtonPressed(CMouseController::LMB))
		{
			UpdateHistory();
			eProperties.bIsSelecting = true;
			DEBUG_MSG("Start Wide Area Fill");
			eProperties.WideAreaSelectionStart = vMousePos;
		}
		else if (cMouseController->IsButtonReleased(CMouseController::LMB))
		{ 
			eProperties.bIsSelecting = false;
			DEBUG_MSG("End Wide Area Fill");
			AreaFill();
		}

		if (cMouseController->IsButtonPressed(CMouseController::RMB))
		{
			UpdateHistory();
			eProperties.bIsSelecting = true;
			DEBUG_MSG("Start Wide Area Delete");
			eProperties.WideAreaSelectionStart = vMousePos;
		}
		else if (cMouseController->IsButtonReleased(CMouseController::RMB))
		{
			eProperties.bIsSelecting = false;
			DEBUG_MSG("End Wide Area Delete");
			AreaDelete();
		}

		eProperties.WideAreaSelectionEnd = vMousePos;
		return true;
	}
	else
	{
		eProperties.bIsSelecting = false;
		eProperties.WideAreaSelectionStart = eProperties.WideAreaSelectionEnd = vMousePos;
	}

	if (cMouseController->IsButtonPressed(CMouseController::MMB))
	{
		CopyBlock();
		return false;
	}

	return false;
}

bool CLevelEditorState::FileUtilShortcuts(void)
{
	if (cKeyboardController->IsKeyDown(GLFW_KEY_LEFT_CONTROL))
	{
		if (cKeyboardController->IsKeyPressed(GLFW_KEY_Z))
		{
			Undo(1);
		}
		else if (cKeyboardController->IsKeyPressed(GLFW_KEY_Y))
		{
			Redo(1);
		}
		else if (cKeyboardController->IsKeyPressed(GLFW_KEY_S))
		{
			Save();
		}
	}
	return true;
}

void CLevelEditorState::AreaFill(void)
{
	eProperties.prevActions.push_back("Area Fill");
	eProperties.bSaved = false;

	int startXIndex = 0;
	int endXIndex = 0;
	int startYIndex = 0;
	int	endYIndex = 0;

	eProperties.RecalculateStartEndIndex(startXIndex, endXIndex, startYIndex, endYIndex);

	for (int iRow = startYIndex; iRow <= endYIndex; ++iRow)
	{
		for (int iCol = startXIndex; iCol <= endXIndex; ++iCol)
		{
			if (cLevelEditor->GetCell(iCol, iRow).iTileID == 0)
				cLevelEditor->UpdateCell(iCol, iRow, activeTile);
		}
	}
}

void CLevelEditorState::AreaDelete(void)
{
	eProperties.prevActions.push_back("Area Delete");
	eProperties.bSaved = false;

	int startXIndex = 0;
	int endXIndex = 0;
	int startYIndex = 0;
	int	endYIndex = 0;

	eProperties.RecalculateStartEndIndex(startXIndex, endXIndex, startYIndex, endYIndex);

	for (int iRow = startYIndex; iRow <= endYIndex; ++iRow)
	{
		for (int iCol = startXIndex; iCol <= endXIndex; ++iCol)
		{
			cLevelEditor->UpdateCell(iCol, iRow, 0);
		}
	}
}

void CLevelEditorState::Undo(const int count)
{
	if (eProperties.undoStates.size() == 1)
		return;

	eProperties.bSaved = false;
	eProperties.iUndoCount += count;

	for (int i = 0; i < count; ++i)
	{
		ActionState currentState;
		currentState.m_ThisActionLevel = cLevelEditor->GetCurrentLevel();
		currentState.iWorldWidth = cLevelEditor->iWorldWidth;
		currentState.iWorldHeight = cLevelEditor->iWorldHeight;

		eProperties.redoStates.push_back(currentState);

		if (cLevelGrid->iWorldWidth < currentState.iWorldWidth)
			cLevelEditor->DecreaseXSize();
		if (cLevelGrid->iWorldWidth > currentState.iWorldWidth)
			cLevelEditor->IncreaseXSize();
		if (cLevelGrid->iWorldHeight < currentState.iWorldHeight)
			cLevelEditor->DecreaseYSize();
		if (cLevelGrid->iWorldHeight > currentState.iWorldHeight)
			cLevelEditor->IncreaseYSize();

		cLevelEditor->SetCurrentLevel(eProperties.undoStates.back().m_ThisActionLevel);
		cLevelGrid->iWorldWidth = eProperties.undoStates.back().iWorldWidth;
		cLevelGrid->iWorldHeight = eProperties.undoStates.back().iWorldHeight;

		eProperties.undoStates.pop_back();
	}
}

void CLevelEditorState::Redo(const int count)
{
	if (eProperties.redoStates.size() == 0)
		return;

	eProperties.bSaved = false;
	eProperties.iUndoCount -= count;


	for (int i = 0; i < count; ++i)
	{
		ActionState currentState;
		currentState.m_ThisActionLevel = cLevelEditor->GetCurrentLevel();
		currentState.iWorldWidth = cLevelEditor->iWorldWidth;
		currentState.iWorldHeight = cLevelEditor->iWorldHeight;

		eProperties.undoStates.push_back(currentState);

		if (currentState.iWorldWidth < eProperties.redoStates.back().iWorldWidth)
			cLevelEditor->DecreaseXSize();
		if (currentState.iWorldWidth > eProperties.redoStates.back().iWorldWidth)
			cLevelEditor->IncreaseXSize();
		if (currentState.iWorldHeight < eProperties.redoStates.back().iWorldHeight)
			cLevelEditor->DecreaseYSize();
		if (currentState.iWorldHeight > eProperties.redoStates.back().iWorldHeight)
			cLevelEditor->DecreaseYSize();

		cLevelEditor->SetCurrentLevel(eProperties.redoStates.back().m_ThisActionLevel);
		cLevelGrid->iWorldWidth = eProperties.redoStates.back().iWorldWidth;
		cLevelGrid->iWorldHeight = eProperties.redoStates.back().iWorldHeight;


		eProperties.redoStates.pop_back();
	}
}

void CLevelEditorState::CopyBlock(void)
{
	activeTile = cLevelEditor->GetCell(vMousePos.x, vMousePos.y).iTileID;
}

void CLevelEditorState::Save()
{
	eProperties.bSaved = true;
	cLevelEditor->SaveMap();
}

void CLevelEditorState::Close()
{
	if (!eProperties.bSaved)
		eProperties.bToggleCloseWindow = true;
	else
	{
		DEBUG_MSG("Closing Editor");
		//cSettings->ImGuiProperties.IsViewportEnabled = false;
		//cSettings->m_ImGuiWindow->Update(cSettings->ImGuiProperties);

		cKeyboardController->Reset();
		cMouseController->Reset();
		CGameStateManager::GetInstance()->SetActiveGameState("MenuState");
	}

}

void CLevelEditorState::MoveCamera(double dElapsedTime)
{

	if (cKeyboardController->IsKeyDown(GLFW_KEY_W))
	{
		Camera2D::GetInstance()->UpdateTarget(glm::vec2(Camera2D::GetInstance()->getTarget().x, Camera2D::GetInstance()->getTarget().y + 0.2));
	}
	if (cKeyboardController->IsKeyDown(GLFW_KEY_S))
	{
		Camera2D::GetInstance()->UpdateTarget(glm::vec2(Camera2D::GetInstance()->getTarget().x, Camera2D::GetInstance()->getTarget().y - 0.2));
	}
	if (cKeyboardController->IsKeyDown(GLFW_KEY_A))
	{
		Camera2D::GetInstance()->UpdateTarget(glm::vec2(Camera2D::GetInstance()->getTarget().x - 0.2, Camera2D::GetInstance()->getTarget().y));
	}
	if (cKeyboardController->IsKeyDown(GLFW_KEY_D))
	{
		Camera2D::GetInstance()->UpdateTarget(glm::vec2(Camera2D::GetInstance()->getTarget().x + 0.2, Camera2D::GetInstance()->getTarget().y));
	}

	Camera2D::GetInstance()->UpdatePos(glm::vec2(Math::Clamp(Camera2D::GetInstance()->getCurrPos().x, 0.f, (float)cLevelEditor->iWorldWidth), Math::Clamp(Camera2D::GetInstance()->getCurrPos().y, 0.f, (float)cLevelEditor->iWorldHeight)));
	Camera2D::GetInstance()->UpdateTarget(glm::vec2(Math::Clamp(Camera2D::GetInstance()->getTarget().x, 0.f, (float)cLevelEditor->iWorldWidth), Math::Clamp(Camera2D::GetInstance()->getTarget().y, 0.f, (float)cLevelEditor->iWorldHeight)));
}

void CLevelEditorState::ScaleMap(void)
{
	if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_RIGHT))
	{
		IncreaseXSize();
	}
	else if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_LEFT))
	{
		DecreaseXSize();
	}
	if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_UP))
	{
		IncreaseYSize();
	}
	else if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_DOWN))
	{
		DecreaseYSize();
	}

}

void CLevelEditorState::MouseInput(double dElapsedTime)
{
	if (cMouseController->IsButtonDown(CMouseController::LMB))
	{
		// DEBUG_MSG("x:" << u16vec2FinalMousePosInEditor.x << " y:" << u16vec2FinalMousePosInEditor.y);
		DEBUG_MSG("[x: " << vMousePos.x << ", y: " << vMousePos.y << "] Cell TileID: " << cLevelEditor->GetCell(vMousePos.x, vMousePos.y, false).iTileID);
		if (cLevelEditor->GetCell(vMousePos.x, vMousePos.y).iTileID == 0 && activeTile != 0)
		{
			if (!eProperties.bPlacedBlock)
			{
				UpdateHistory();
				eProperties.prevActions.push_back("Placed Tile");
				eProperties.bPlacedBlock = true;
			}

			eProperties.bSaved = false;
			cLevelEditor->UpdateCell(vMousePos.x, vMousePos.y, activeTile);
		}
	}
	if (cMouseController->IsButtonReleased(CMouseController::LMB))
	{
		eProperties.bPlacedBlock = false;
		if (cLevelEditor->GetCell(vMousePos.x, vMousePos.y).iTileID > OBJECT_TYPE::INTERACTABLE_START && cLevelEditor->GetCell(vMousePos.x, vMousePos.y).iTileID < OBJECT_TYPE::INTERACTABLE_TOTAL)
		{
			eProperties.bOpenIDPopup = true;
			eProperties.iCurrentInteractableID = cLevelEditor->GetCell(vMousePos.x, vMousePos.y).iInteractableID;
			eProperties.BlockPosition = vMousePos;
		}
	}

	if (cMouseController->IsButtonDown(CMouseController::RMB))
	{
		if (cLevelEditor->GetCell(vMousePos.x, vMousePos.y).iTileID != 0)
		{
			if (!eProperties.bDeletedBlock)
			{
				UpdateHistory();
				eProperties.prevActions.push_back("Removed Tile");
				eProperties.bDeletedBlock = true;
			}


			eProperties.bSaved = false;
			cLevelEditor->UpdateCell(vMousePos.x, vMousePos.y, 0);
		}
	}
	if (cMouseController->IsButtonReleased(CMouseController::RMB))
	{
		eProperties.bDeletedBlock = false;
	}
	if (cMouseController->GetMouseScrollStatus(CMouseController::SCROLL_TYPE_YOFFSET) > 0)
	{
		Camera2D::GetInstance()->UpdateZoom(Camera2D::GetInstance()->getTargetZoom() + 0.1f);
	}
	if (cMouseController->GetMouseScrollStatus(CMouseController::SCROLL_TYPE_YOFFSET) < 0)
	{
		Camera2D::GetInstance()->UpdateZoom(Camera2D::GetInstance()->getTargetZoom() - 0.1f);
	}
}

void CLevelEditorState::GenerateQuadVAO()
{
	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates. NOTE that this plane is now much smaller and at the top of the screen
	   // positions   // texCoords
	   -1.0f,  1.0f,  0.0f, 1.0f,
	   -1.0f,  -1.0f,  0.0f, 0.0f,
		1.0f,  -1.0f,  1.0f, 0.0f,

	   -1.0f,  1.0f,  0.0f, 1.0f,
		1.0f,  -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void CLevelEditorState::GenerateFBO()
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// create a color attachment texture
	glGenTextures(1, &textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cSettings->iWindowWidth, cSettings->iWindowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1200, 900); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO); // now actually attach it
																								  // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		DEBUG_MSG("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CLevelEditorState::RenderQuad(unsigned int iTextureID)
{
	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, iTextureID);
	// glDrawArrays(GL_TRIANGLES, 0, 6);
	cursor->Render();
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool CLevelEditorState::ImGuiRender()
{
	//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	//ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	//ImGui::Begin("GameView", NULL, windowFlags);
	//{
	//	ImGui::Image((void*)(intptr_t)textureColorBuffer, { 1200, (float)cSettings->iWindowHeight }, { 0, 1 }, { 1, 0 });
	//	DEBUG_MSG(ImGui::GetWindowPos().x << " " << ImGui::GetWindowPos().y);
	//}
	//ImGui::End();
	//ImGui::PopStyleVar();

	// Main Menu
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			std::string saveString = "Save " + cLevelEditor->GetCurrentLevelData().LevelName;
			std::string closeString = "Close " + cLevelEditor->GetCurrentLevelData().LevelName;

			if (ImGui::MenuItem(saveString.c_str(), "CTRL+S")) Save();
			if (ImGui::MenuItem(closeString.c_str(), "ESCAPE")) Close();

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z")) Undo(1);
			if (ImGui::MenuItem("Redo", "CTRL+Y")) Redo(1);

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("History")) eProperties.bToggleHistoryWindow = !eProperties.bToggleHistoryWindow;

			ImGui::EndMenu();
		}

		if (!eProperties.bSaved)
		{
			ImGuiWindowFlags TextWindowFlags = ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoScrollWithMouse |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoBackground |
				ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoFocusOnAppearing |
				ImGuiWindowFlags_NoBringToFrontOnFocus |
				ImGuiWindowFlags_NoNavInputs |
				ImGuiWindowFlags_NoNavFocus;

			ImGuiIO& io = ImGui::GetIO();

			ImGui::SetCursorPos(ImVec2((float)cSettings->iWindowWidth - 110.f, 0));
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Unsaved Changes");
		}

		ImGui::EndMainMenuBar();
	}

	ImGui::SetNextWindowPos(ImVec2(0, 19));
	ImGui::SetNextWindowSize(ImVec2(260.f, (float)cSettings->iWindowHeight));

	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize;

	// Editor Window
	ImGui::SetNextWindowCollapsed(!eProperties.bToggleEditorWindow);
	eProperties.bToggleEditorWindow = false;
	if (ImGui::Begin("Editor", NULL, windowFlags))
	{
		eProperties.bToggleEditorWindow = true;
		ImGui::TextColored(ImVec4(1.f, 1.f, 0, 1.f), "Map Size");
		std::string xSize = "X: " + std::to_string(cLevelEditor->iWorldWidth);
		std::string ySize = "Y: " + std::to_string(cLevelEditor->iWorldHeight);

		ImGui::Text(xSize.c_str()); 
		ImGui::SameLine(); if (ImGui::Button("-##X")) DecreaseXSize();
		ImGui::SameLine(); if (ImGui::Button("+##X")) IncreaseXSize();
		ImGui::Text(ySize.c_str());					  
		ImGui::SameLine(); if (ImGui::Button("-##Y")) DecreaseYSize();
		ImGui::SameLine(); if (ImGui::Button("+##Y")) IncreaseYSize();

		ImGui::NewLine();

		ImGui::TextColored(ImVec4(1.f, 1.f, 0, 1.f), "Background Mesh");
		if (cLevelEditor->cBackgroundEntity)
			ImGui::TextWrapped(cLevelEditor->backgroundPath.c_str());
		else 
			ImGui::Text("No Background");

		if (ImGui::Button("Change Background"))
		{
			cLevelEditor->backgroundPath = FileDialog::OpenFile();

			if (cLevelEditor->backgroundPath != "")
			{
				delete cLevelEditor->cBackgroundEntity;
				cLevelEditor->cBackgroundEntity = new CBackgroundEntity(cLevelEditor->backgroundPath);
				if (!cLevelEditor->cBackgroundEntity->Init())
				{
					delete cLevelEditor->cBackgroundEntity;
					cLevelEditor->cBackgroundEntity = nullptr;
				}
			}
		}

		ImGui::NewLine();

		ImGui::PushItemWidth(150);
		ImGui::TextColored(ImVec4(1.f, 1.f, 0, 1.f), "Parallax Allowance");
		ImGui::SliderFloat("X##Parallax", &cLevelEditor->vAllowanceScale.x, 0.f, 1.f);
		ImGui::SliderFloat("Y##Parallax", &cLevelEditor->vAllowanceScale.y, 0.f, 1.f);

		ImGui::NewLine();

		ImGui::TextColored(ImVec4(1, 1, 0, 1), "BG Size");
		ImGui::SliderFloat("X##BG", &cLevelEditor->vUVCoords.x, 2.f, 5.f);
		ImGui::SliderFloat("Y##BG", &cLevelEditor->vUVCoords.y, 2.f, 5.f);

		ImGui::PopItemWidth();

		if (ImGui::BeginChild("Tile List"))
		{
			if (ImGui::BeginTabBar("Editor Tab"))
			{
				if (ImGui::BeginTabItem("Tiles"))
				{
					RenderImGuiEditorButtons(OBJECT_TYPE::TILE_START, OBJECT_TYPE::TILE_TOTAL);
					ImGui::EndTabItem();
				}	

				if (ImGui::BeginTabItem("Obstacles"))
				{
					RenderImGuiEditorButtons(OBJECT_TYPE::OBSTACLES_START, OBJECT_TYPE::OBSTACLES_TOTAL);
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Interactables"))
				{
					RenderImGuiEditorButtons(OBJECT_TYPE::INTERACTABLE_START, OBJECT_TYPE::INTERACTABLE_TOTAL);
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Enemies"))
				{
					RenderImGuiEditorButtons(OBJECT_TYPE::ENEMIES_START, OBJECT_TYPE::ENEMIES_TOTAL);
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Player"))
				{
					RenderImGuiEditorButtons(0, OBJECT_TYPE::PLAYER_TILE + 1);
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
		ImGui::EndChild();
		// DEBUG_MSG(ImGui::GetWindowPos().x << " " << ImGui::GetWindowPos().y);
	}
	ImGui::End();

	windowFlags = 0;

	// Action History Window
	ImGui::SetNextWindowSize(ImVec2(150, 250), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2((float)cSettings->iWindowWidth - 150.f, 19), ImGuiCond_Once);
	if (eProperties.bToggleHistoryWindow)
	{
		if (ImGui::Begin("History", &eProperties.bToggleHistoryWindow, windowFlags))
		{
			if (ImGui::BeginChild("ActionHistory"))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.1f, 0.5f));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
				for (unsigned i = 0; i < eProperties.prevActions.size(); ++i)
				{
					if (i < eProperties.prevActions.size() - eProperties.iUndoCount)
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.40f));
					else
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

					if (ImGui::Button((eProperties.prevActions[i] + "##" + std::to_string(i)).c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 25)))
					{

						// eProperties.iUndoCount = eProperties.prevActions.size() - i;
						// Undo
						if ((i + 1) < eProperties.prevActions.size() - eProperties.iUndoCount)
							Undo(eProperties.prevActions.size() - (i + 1) - eProperties.iUndoCount);

						// Redo
						else if ((i + 1) > eProperties.prevActions.size() - eProperties.iUndoCount)
							Redo((i + 1) - (eProperties.prevActions.size() - eProperties.iUndoCount));
					}

					ImGui::PopStyleColor();
				}
				ImGui::PopStyleVar(2);

				if (eProperties.bHistoryUpdated)
				{
					eProperties.bHistoryUpdated = false;
					ImGui::SetScrollHereY(1.f);
				}

			}
			ImGui::EndChild();
		}
		ImGui::End();
	}

	windowFlags =
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse;

	// Close Window Alert
	ImGui::SetNextWindowPos(ImVec2((cSettings->iWindowWidth - 250) * 0.5f, (cSettings->iWindowHeight - 105) * 0.5f));

	ImGui::SetNextWindowSize(ImVec2(250, 105));
	if (eProperties.bToggleCloseWindow)
	{
		if (ImGui::Begin("Alert", &(eProperties.bToggleCloseWindow), windowFlags))
		{
			DEBUG_MSG(ImGui::GetWindowSize().x << " " << ImGui::GetWindowSize().y);
			ImGui::TextWrapped("You have unsaved changes. Do you want to discard them or cancel?");

			ImGui::NewLine();

			if (ImGui::Button("Discard"))
			{
				eProperties.bSaved = true;
				Close();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel")) eProperties.bToggleCloseWindow = false;
			ImGui::End();
		}
	}

	if (eProperties.bOpenIDPopup)
	{
		ImGui::OpenPopup("InteractableID");
	}

	// Interactable ID Popup
	if (ImGui::BeginPopup("InteractableID"))
	{
		ImGui::SetNextItemWidth(100);
		ImGui::InputInt("Interactable ID", &eProperties.iCurrentInteractableID);
		if (ImGui::Button("Done"))
		{
			cLevelEditor->UpdateCell((unsigned int)eProperties.BlockPosition.x, (unsigned int)eProperties.BlockPosition.y, cLevelEditor->GetCell((unsigned int)eProperties.BlockPosition.x, (unsigned int)eProperties.BlockPosition.y).iTileID, eProperties.iCurrentInteractableID);
			eProperties.bOpenIDPopup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	

	return true;
}

void CLevelEditorState::RenderCursor()
{
	// Activate blending mode
	if (activeTile != 0)
	{
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindVertexArray(quadVAO);

		glm::vec2 offset = glm::vec2(float(CSettings::GetInstance()->NUM_TILES_XAXIS / 2.f), float(CSettings::GetInstance()->NUM_TILES_YAXIS / 2.f));
		glm::vec2 cameraPos = Camera2D::GetInstance()->getCurrPos();

		if (!eProperties.bIsSelecting)
		{
			glm::vec2 objCamPos = glm::vec2(vMousePos.x, vMousePos.y) - cameraPos + offset;
			glm::vec2 actualPos = CSettings::GetInstance()->ConvertIndexToUVSpace(objCamPos) * Camera2D::GetInstance()->getZoom();

			transform = glm::mat4(1.f);
			transform = glm::translate(transform, glm::vec3(actualPos.x, actualPos.y, 0.f));
			transform = glm::scale(transform, glm::vec3(Camera2D::GetInstance()->getZoom()));
			transform = glm::scale(transform, glm::vec3(CSettings::GetInstance()->TILE_WIDTH, CSettings::GetInstance()->TILE_HEIGHT, 1.f));

			CShaderManager::GetInstance()->Use("2DColorShader");
			CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);
			CShaderManager::GetInstance()->activeShader->setVec4("runtime_color", glm::vec4(1.f, 1.f, 1.f, 0.6f));
			CShaderManager::GetInstance()->activeShader->setMat4("transform", transform);
			
			RenderQuad(CTextureManager::GetInstance()->MapOfTextureIDs.at(activeTile));
		}
		else
		{
			int startXIndex = 0;
			int endXIndex = 0;
			int startYIndex = 0;
			int	endYIndex = 0;

			eProperties.RecalculateStartEndIndex(startXIndex, endXIndex, startYIndex, endYIndex);

			for (int iRow = startYIndex; iRow <= endYIndex; ++iRow)
			{
				for (int iCol = startXIndex; iCol <= endXIndex; ++iCol)
				{
					glm::vec2 objCamPos = glm::vec2(iCol, iRow) - cameraPos + offset;
					glm::vec2 actualPos = CSettings::GetInstance()->ConvertIndexToUVSpace(objCamPos) * Camera2D::GetInstance()->getZoom();

					transform = glm::mat4(1.f);
					transform = glm::translate(transform, glm::vec3(actualPos.x, actualPos.y, 0.f));
					transform = glm::scale(transform, glm::vec3(Camera2D::GetInstance()->getZoom()));
					transform = glm::scale(transform, glm::vec3(CSettings::GetInstance()->TILE_WIDTH, CSettings::GetInstance()->TILE_HEIGHT, 1.f));

					CShaderManager::GetInstance()->Use("2DColorShader");
					CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);
					CShaderManager::GetInstance()->activeShader->setVec4("runtime_color", glm::vec4(1.f, 1.f, 1.f, 0.6f));
					CShaderManager::GetInstance()->activeShader->setMat4("transform", transform);

					RenderQuad(CTextureManager::GetInstance()->MapOfTextureIDs.at(activeTile));
				}
			}
		}

		glBindVertexArray(0);

		// Disable blending
		glDisable(GL_BLEND);
	}
}


void CLevelEditorState::IncreaseXSize(void)
{
	if (cLevelEditor->IncreaseXSize())
	{
		UpdateHistory();
		eProperties.prevActions.push_back("Increase Level Width");
		cLevelGrid->iWorldWidth++;
	}
}

void CLevelEditorState::DecreaseXSize(void)
{
	if (cLevelEditor->DecreaseXSize())
	{
		UpdateHistory();
		eProperties.prevActions.push_back("Decrease Level Width");
		cLevelGrid->iWorldWidth--;
	}
}

void CLevelEditorState::IncreaseYSize(void)
{
	if (cLevelEditor->IncreaseYSize())
	{
		UpdateHistory();
		eProperties.prevActions.push_back("Increase Level Height");
		cLevelGrid->iWorldHeight++;
	}
}

void CLevelEditorState::DecreaseYSize(void)
{
	if (cLevelEditor->DecreaseYSize())
	{
		UpdateHistory();
		eProperties.prevActions.push_back("Decrease Level Height");
		cLevelGrid->iWorldHeight--;
	}
}

void CLevelEditorState::UpdateHistory(void)
{
	eProperties.bHistoryUpdated = true;

	for (int i = 0; i < eProperties.iUndoCount; ++i)
	{
		eProperties.prevActions.pop_back();
	}

	eProperties.iUndoCount = 0;

	ActionState currentState;
	currentState.m_ThisActionLevel = cLevelEditor->GetCurrentLevel();
	currentState.iWorldWidth = cLevelGrid->iWorldWidth;
	currentState.iWorldHeight = cLevelGrid->iWorldHeight;

	eProperties.undoStates.push_back(currentState);
	eProperties.redoStates.clear();
}

void CLevelEditorState::RenderImGuiEditorButtons(uint32_t iStart, uint32_t iEnd)
{
	const int iMaxButtonsPerRow = 6;
	int iCounter = 1;

	for (unsigned i = iStart + 1; i < iEnd; ++i)
	{
		if (CTextureManager::GetInstance()->MapOfTextureIDs.find(i) == CTextureManager::GetInstance()->MapOfTextureIDs.end())
			continue;

		if (iCounter <= iMaxButtonsPerRow && iCounter != 1)
		{
			ImGui::SameLine();
		}
		else
			iCounter = 1;

		if (activeTile == i)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.40f));
		else
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));

		if (ImGui::ImageButton((void*)CTextureManager::GetInstance()->MapOfTextureIDs.at(i), ImVec2(25.f, 25.f), ImVec2(0, 1), ImVec2(1, 0)))
		{
			activeTile = i;
		}
		ImGui::PopStyleColor();
		++iCounter;

	}
}
