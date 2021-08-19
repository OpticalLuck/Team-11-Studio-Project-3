// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <GLFW/glfw3.h>

#include "LevelEditorState.h"

// Include CGameStateManager
#include "GameStateManager.h"

// Include CKeyboardController
#include "Inputs/KeyboardController.h"

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
	: cKeyboardInputHandler(NULL)
	, cLevelEditor(NULL)
	, cMouseController(NULL)
	, cSettings(NULL)
	, cLevelGrid(NULL)
	, activeTile(0)
	, transform(1.f)
	, cursor(NULL)
	, vMousePos(0.f)
{
}
/**
 @brief Destructor
 */
CLevelEditorState::~CLevelEditorState(void)
{
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);

	cKeyboardInputHandler = NULL;
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

	cSettings = CSettings::GetInstance();
	cSettings->screenSize = CSettings::SSIZE_1024x768;
	CSettings::GetInstance()->UpdateWindowSize();

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);

	// GenerateQuadVAO();
	// GenerateFBO();

	cursor = CMeshBuilder::GenerateQuad();

	cLevelEditor = CLevelEditor::GetInstance();
	cLevelGrid = CLevelGrid::GetInstance();
	cLevelGrid->Init(cLevelEditor->iWorldWidth, cLevelEditor->iWorldHeight);

	Camera2D::GetInstance()->Reset();
	Camera2D::GetInstance()->UpdateTarget(glm::vec2(cSettings->NUM_TILES_XAXIS * 0.5, cSettings->NUM_TILES_YAXIS * 0.5));

	cMouseController = CMouseController::GetInstance();

	cKeyboardInputHandler = CKeyboardInputHandler::GetInstance();
	cKeyboardInputHandler->Init();

	return true;
}

/**
 @brief Update this class instance
 */
bool CLevelEditorState::Update(const double dElapsedTime)
{
	Camera2D::GetInstance()->Update(dElapsedTime);

	vMousePos = Camera2D::GetInstance()->GetCursorPosInWorldSpace(0);

	vMousePos.x = Math::Clamp((float)vMousePos.x, 0.f, (float)cLevelEditor->iWorldWidth - 1);
	vMousePos.y = Math::Clamp((float)vMousePos.y, 0.f, (float)cLevelEditor->iWorldHeight - 1);

	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "Loading PauseState" << endl;
		CGameStateManager::GetInstance()->SetPauseGameState("PauseState");
		return true;
	}

	MoveCamera();
	ScaleMap();

	if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_4))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_3))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}


	if (!CImGuiWindow::GetInstance()->WantCaptureMouse())
		MouseInput();

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

	cLevelGrid->PreRender();
	cLevelGrid->Render();
	cLevelGrid->PostRender();

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	RenderCursor();
	ImGuiRender();
}

/**
 @brief Destroy this class instance
 */
void CLevelEditorState::Destroy(void)
{
	cout << "CLevelEditorState::Destroy()\n" << endl;
}


void CLevelEditorState::MoveCamera(void)
{
	if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_W))
	{
		Camera2D::GetInstance()->UpdateTarget(glm::vec2(Camera2D::GetInstance()->getTarget().x, Camera2D::GetInstance()->getTarget().y + 0.2));
	}
	if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_S))
	{
		Camera2D::GetInstance()->UpdateTarget(glm::vec2(Camera2D::GetInstance()->getTarget().x, Camera2D::GetInstance()->getTarget().y - 0.2));
	}
	if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_A))
	{
		Camera2D::GetInstance()->UpdateTarget(glm::vec2(Camera2D::GetInstance()->getTarget().x - 0.2, Camera2D::GetInstance()->getTarget().y));
	}
	if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_D))
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
		if (cLevelEditor->IncreaseXSize())
			cLevelGrid->iWorldWidth++;
	}
	else if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_LEFT))
	{
		if (cLevelEditor->DecreaseXSize())
			cLevelGrid->iWorldWidth--;
	}
	if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_UP))
	{
		if (cLevelEditor->IncreaseYSize())
			cLevelGrid->iWorldHeight++;
	}
	else if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_DOWN))
	{
		if (cLevelEditor->DecreaseYSize())
			cLevelGrid->iWorldHeight--;
	}

}

void CLevelEditorState::MouseInput(void)
{
	if (cMouseController->IsButtonDown(CMouseController::LMB))
	{
		// DEBUG_MSG("x:" << u16vec2FinalMousePosInEditor.x << " y:" << u16vec2FinalMousePosInEditor.y);
		DEBUG_MSG("[x: " << vMousePos.x << ", y: " << vMousePos.y << "] Cell TileID: " << cLevelEditor->GetCell(vMousePos.x, vMousePos.y, false).iTileID);
		if (cLevelEditor->GetCell(vMousePos.x, vMousePos.y, false).iTileID == 0)
		{
			cLevelEditor->UpdateCell(vMousePos.x, vMousePos.y, activeTile, false);
		}
	}

	if (cMouseController->IsButtonDown(CMouseController::RMB))
	{
		// DEBUG_MSG("x:" << u16vec2FinalMousePosInEditor.x << " y:" << u16vec2FinalMousePosInEditor.y);
		if (cLevelEditor->GetCell(vMousePos.x, vMousePos.y, false).iTileID != 0)
		{
			cLevelEditor->UpdateCell(vMousePos.x, vMousePos.y, 0, false);
		}
	}

	if (cMouseController->GetMouseScrollStatus(CMouseController::SCROLL_TYPE_YOFFSET) > 0)
	{
		Camera2D::GetInstance()->UpdateZoom(Camera2D::GetInstance()->getTargetZoom() + 0.1);
	}
	if (cMouseController->GetMouseScrollStatus(CMouseController::SCROLL_TYPE_YOFFSET) < 0)
	{
		Camera2D::GetInstance()->UpdateZoom(Camera2D::GetInstance()->getTargetZoom() - 0.1);
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

void CLevelEditorState::ImGuiRender()
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

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			std::string saveString = "Save " + cLevelEditor->GetCurrentLevel().LevelName;
			std::string closeString = "Close " + cLevelEditor->GetCurrentLevel().LevelName;

			if (ImGui::MenuItem(saveString.c_str(), "CTRL+S")) cLevelEditor->SaveMap();
			if (ImGui::MenuItem(closeString.c_str(), "ESCAPE"))
			{
				DEBUG_MSG("Closing Editor");
				CGameStateManager::GetInstance()->SetActiveGameState("MenuState");
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	ImGui::SetNextWindowPos(ImVec2(0, 0 + 19));
	ImGui::SetNextWindowSize(ImVec2(250, cSettings->iWindowHeight));

	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize;

	if (ImGui::Begin("Editor", NULL, windowFlags))
	{
		ImGui::TextColored(ImVec4(1.f, 1.f, 0, 1.f), "Map Size");
		std::string xSize = "X: " + std::to_string(cLevelEditor->iWorldWidth);
		std::string ySize = "Y: " + std::to_string(cLevelEditor->iWorldHeight);
		ImGui::Text(xSize.c_str());
		ImGui::Text(ySize.c_str());

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
		ImGui::SliderFloat("Parallax X", &cLevelEditor->vAllowanceScale.x, 0.f, 1.f);
		ImGui::SliderFloat("Parallax Y", &cLevelEditor->vAllowanceScale.y, 0.f, 1.f);

		ImGui::NewLine();

		ImGui::TextColored(ImVec4(1, 1, 0, 1), "BG Size");
		ImGui::SliderFloat("BG X", &cLevelEditor->vUVCoords.x, 2.f, 5.f);
		ImGui::SliderFloat("BG Y", &cLevelEditor->vUVCoords.y, 2.f, 5.f);
		ImGui::PopItemWidth();

		if (ImGui::BeginChild("Tile List"))
		{
			if (ImGui::BeginTabBar("Editor Tab"))
			{
				if (ImGui::BeginTabItem("Tiles"))
				{
				
					const int iMaxButtonsPerRow = 6;
					int iCounter = 0;
					for (int i = TILE_GROUND; i < OBJECT_TOTAL; ++i)
					{
						if (CTextureManager::GetInstance()->MapOfTextureIDs.find(i) == CTextureManager::GetInstance()->MapOfTextureIDs.end())
							continue;

						if (iCounter <= iMaxButtonsPerRow && iCounter != 0)
						{
							ImGui::SameLine();
						}
						else
							iCounter = 0;
						
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
					ImGui::EndTabItem();
				}	

				if (ImGui::BeginTabItem("Interactables"))
				{
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Enemies"))
				{
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
		ImGui::EndChild();
		// DEBUG_MSG(ImGui::GetWindowPos().x << " " << ImGui::GetWindowPos().y);
	}
	ImGui::End();

}

void CLevelEditorState::RenderCursor()
{
	// Activate blending mode
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindVertexArray(quadVAO);

	glm::vec2 offset = glm::vec2(float(CSettings::GetInstance()->NUM_TILES_XAXIS / 2.f), float(CSettings::GetInstance()->NUM_TILES_YAXIS / 2.f));
	glm::vec2 cameraPos = Camera2D::GetInstance()->getCurrPos();

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

	if (activeTile != 0)
		RenderQuad(CTextureManager::GetInstance()->MapOfTextureIDs.at(activeTile));
	
	glBindVertexArray(0);

	// Disable blending
	glDisable(GL_BLEND);
}
