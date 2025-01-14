// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <Windows.h>
#include <GLFW/glfw3.h>

#include "PlayGameState.h"

// Include ImageLoader
#include "System\ImageLoader.h"

// Include CKeyboardController
#include "Inputs/KeyboardController.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CPlayGameState::CPlayGameState(void)
	: cScene2D(NULL)
	, cInputHandler(NULL)
	, cSettings(NULL)
	, cTextureManager(NULL)
	, cPlayerInventory(NULL)
	, cPlayer(NULL)
	, cEntityManager(NULL)
	, cKeyboardController(NULL)
	, m_fProgressBar(0.0f)
	, fInterval(0.f)
	, iMinutes(0)
	, iSeconds(0)
	, transformX(0.f)
	, transformY(0.f)
	, cCamera(NULL)
	, cSoundController(NULL)
	, displayHP(0.f)
	, displayTimer(0.f)
	, cFPSCounter(NULL)
	, cMap2D(NULL)
	, iTimer2(0)
	, iInterval2(0)
	, iOffsetX(0)
	, iOffsetY(0)
	, fFadeCount(0)
{

}

/**
 @brief Destructor
 */
CPlayGameState::~CPlayGameState(void)
{
	Destroy();
}

/**
 @brief Init this class instance
 */
bool CPlayGameState::Init(void)
{
	cout << "CPlayGameState::Init()\n" << endl;

	// Initialise the cScene2D instance
	cScene2D = CScene2D::GetInstance();
	if (cScene2D->Init(levelPath) == false)
	{
		cout << "Failed to load Scene2D" << endl;
		return false;
	}

	cInputHandler = CInputHandler::GetInstance();
	cInputHandler->Init();

	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Store the CFPSCounter singleton instance here
	cFPSCounter = CFPSCounter::GetInstance();

	cPlayerInventory = CInventoryManager::GetInstance()->Get("Player");
	cTextureManager = CTextureManager::GetInstance();
	
	cEntityManager = CEntityManager::GetInstance();
	cPlayer = cEntityManager->GetPlayer();
	displayHP = (float)cPlayer->GetHealth();
	cGameStateManager = CGameStateManager::GetInstance();

	// Create and initialise the Map 2D
	cMap2D = CMap2D::GetInstance();
	cCamera = Camera2D::GetInstance();
	cCamera->UpdateZoom(1.4f);

	cSoundController = CSoundController::GetInstance();
	CImageLoader* il = CImageLoader::GetInstance();
	background.fileName = "Image\\Backgrounds\\UI.png";
	background.textureID = il->LoadTextureGetID(background.fileName.c_str(), false);
	instruction.fileName = "Image\\Backgrounds\\instuction.png";
	instruction.textureID = il->LoadTextureGetID(instruction.fileName.c_str(), false);

	iTimer2 = 0;
	iInterval2 = 300;
	bInstruction = true;
	bIncrease = true;
	bDecrease = false;
	fFadeCount = 0.f;

	iOffsetX = 0;
	iOffsetY = 0;

	//optionButtonData.fileName = "Image\\GUI\\OptionButton.png";
	//optionButtonData.textureID = il->LoadTextureGetID(optionButtonData.fileName.c_str(), false);

	return true;
}

/**
 @brief Update this class instance
 */
bool CPlayGameState::Update(const double dElapsedTime)
{
	cCamera->UpdateTarget(cPlayer->vTransform);
	cCamera->Update((float)dElapsedTime);
	cCamera->ClampCamPos(cMap2D->GetLevelLimit());
	//cSoundController->Playlist();

	if (!bInstruction)
	{
		if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE))
		{
			cGameStateManager->bOption = true;
			CGameStateManager::GetInstance()->SetPauseGameState("PauseState");
		}

		if (!cGameStateManager->bOption)
		{
			cInputHandler->Update(dElapsedTime);
			// Call the cScene2D's Update method
			if (cScene2D->Update(dElapsedTime) == false)
			{
				// Reset the CKeyboardController
				CKeyboardController::GetInstance()->Reset();

				// Load the menu state
				cout << "Loading PauseState" << endl;
				CGameStateManager::GetInstance()->SetActiveGameState("MenuState");
				return true;
			}
		}
	}

	if (cScene2D->isCompleted == true)
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		CSoundController::GetInstance()->StopPlayBack();

		// Load the menu state
		cout << "Loading GameOverState" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("GameOverState");
		return true;
	}
	
		
	if (cPlayer->GetHealth() <= 0)
	{
		for (int i = 0; i < cPlayer->GetInventory()->GetNumofUniqueItems(); ++i)
			cPlayer->GetInventory()->GetItem(i).iCount = cPlayer->m_CheckpointState.m_CheckpointInventoryState->GetItem(i).iCount;
		
		cPlayer->vTransform = cPlayer->m_CheckpointState.m_CheckpointPosition;
		cPlayer->SetHealth(cPlayer->m_CheckpointState.m_CheckpointHP);
		--cPlayer->iLives;

		return true;
	}
	else if (cPlayer->iLives < 0)
	{
		CGameStateManager::GetInstance()->SetActiveGameState("GameOverState");
		CSoundController::GetInstance()->StopPlayBack();
		return true;
	}


	if (bInstruction)
	{
		if (bIncrease)
			fFadeCount += 0.01f;
		if (bDecrease)
			fFadeCount -= 0.01f;
		if (fFadeCount >= 1.f)
		{
			bIncrease = false;
			bDecrease = true;
		}
		if (fFadeCount <= 0.f)
		{
			bIncrease = true;
			bDecrease = false;
		}
		if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_SPACE))
		{
			fFadeCount = 0;
			bIncrease = true;
			bDecrease = false;
			bInstruction = false;
		}
	}


	return true;
}

/**
 @brief Render this class instance
 */
void CPlayGameState::Render(void)
{
	// Call the cScene2D's Pre-Render method
	cScene2D->PreRender();

	// Call the cScene2D's Render method
	cScene2D->Render();

	// Call the cScene2D's PostRender method
	cScene2D->PostRender();
}

/**
 @brief Destroy this class instance
 */
void CPlayGameState::Destroy(void)
{
	DEBUG_MSG("CPlayGameState::Destroy()\n");

	// Destroy the cScene2D instance
	if (cScene2D)
	{
		cScene2D->Destroy();
		cScene2D = NULL;
	}
	if (cInputHandler)
	{
		cInputHandler->Reset();
		cInputHandler->Destroy();
		cInputHandler = NULL;
	}
	CInventoryManager::GetInstance()->Destroy();
}

bool CPlayGameState::ImGuiRender()
{
	if (bInstruction)
	{
		if (cSettings->GetWindowSize().x == 800 && cSettings->GetWindowSize().y == 600)
		{
			iOffsetX = -3;
			iOffsetY = 40;
		}
		else if (cSettings->GetWindowSize().x == 1024 && cSettings->GetWindowSize().y == 768)
		{
			iOffsetX = -3;
			iOffsetY = 30;
		}
		instruction_window = 0;
		instruction_window |= ImGuiWindowFlags_NoBackground;
		instruction_window |= ImGuiWindowFlags_NoTitleBar;
		instruction_window |= ImGuiWindowFlags_NoMove;
		instruction_window |= ImGuiWindowFlags_NoResize;
		instruction_window |= ImGuiWindowFlags_NoCollapse;

		float relativeScale_x = cSettings->iWindowWidth / 800.0f;
		float relativeScale_y = cSettings->iWindowHeight / 600.0f;

		relativeScale_x = Math::Max(1.f, relativeScale_x);
		relativeScale_y = Math::Max(1.f, relativeScale_y);
		ImGui::SetNextWindowFocus();

		ImGui::Begin("Instuction window", NULL, instruction_window);
		ImGui::SetWindowPos(ImVec2(-10 + iOffsetX ,cSettings->iWindowHeight * 0.075 - 100 + iOffsetY));
		ImGui::SetWindowSize(ImVec2(1200.0f * relativeScale_x, 800.0f * relativeScale_y));
		ImGui::Image((void*)(intptr_t)instruction.textureID, ImVec2(810 * relativeScale_x, 610 * relativeScale_y), ImVec2(0, 0), ImVec2(1, 1));
		ImGui::End();
		ImGui::SetNextWindowFocus();

		ImGui::Begin("Continue message", NULL, instruction_window);
		ImGui::SetWindowSize(ImVec2(1200.0f * relativeScale_x, 200.0f * relativeScale_y));
		ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.25f, cSettings->iWindowHeight * 0.87f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, .0f, 0.0f, 0.f + fFadeCount));
		ImGui::Text("Press Spacebar to continue");
		ImGui::SetWindowFontScale(1.1 * (relativeScale_x + relativeScale_y));
		ImGui::PopStyleColor();
		ImGui::End();
	}
	else
	{
		if (!cGameStateManager->bOption)
		{
			// Define the window flags
			window_flags = 0;
			window_flags |= ImGuiWindowFlags_NoBackground;
			window_flags |= ImGuiWindowFlags_NoTitleBar;
			window_flags |= ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoResize;
			window_flags |= ImGuiWindowFlags_NoCollapse;

			health_window = 0;
			health_window |= ImGuiWindowFlags_AlwaysAutoResize;
			health_window |= ImGuiWindowFlags_NoBackground;
			health_window |= ImGuiWindowFlags_NoTitleBar;
			health_window |= ImGuiWindowFlags_NoMove;
			health_window |= ImGuiWindowFlags_NoResize;
			health_window |= ImGuiWindowFlags_NoCollapse;
			health_window |= ImGuiWindowFlags_NoScrollbar;

			enemyHealth_window = 0;
			enemyHealth_window |= ImGuiWindowFlags_AlwaysAutoResize;
			enemyHealth_window |= ImGuiWindowFlags_NoBackground;
			enemyHealth_window |= ImGuiWindowFlags_NoTitleBar;
			enemyHealth_window |= ImGuiWindowFlags_NoMove;
			enemyHealth_window |= ImGuiWindowFlags_NoResize;
			enemyHealth_window |= ImGuiWindowFlags_NoCollapse;
			enemyHealth_window |= ImGuiWindowFlags_NoScrollbar;


			cloneHealth_window = 0;
			cloneHealth_window |= ImGuiWindowFlags_AlwaysAutoResize;
			cloneHealth_window |= ImGuiWindowFlags_NoBackground;
			cloneHealth_window |= ImGuiWindowFlags_NoTitleBar;
			cloneHealth_window |= ImGuiWindowFlags_NoMove;
			cloneHealth_window |= ImGuiWindowFlags_NoResize;
			cloneHealth_window |= ImGuiWindowFlags_NoCollapse;
			cloneHealth_window |= ImGuiWindowFlags_NoScrollbar;


			UI_window = 0;
			UI_window |= ImGuiWindowFlags_NoTitleBar;
			UI_window |= ImGuiWindowFlags_NoScrollbar;
			UI_window |= ImGuiWindowFlags_NoBackground;
			UI_window |= ImGuiWindowFlags_NoMove;
			UI_window |= ImGuiWindowFlags_NoCollapse;
			UI_window |= ImGuiWindowFlags_NoNav;
			UI_window |= ImGuiWindowFlags_NoMouseInputs;
			UI_window |= ImGuiWindowFlags_NoResize;

			// Calculate the relative scale to our default windows width
			float relativeScale_x = cSettings->iWindowWidth / 800.0f;
			float relativeScale_y = cSettings->iWindowHeight / 600.0f;

			relativeScale_x = Math::Max(1.f, relativeScale_x);
			relativeScale_y = Math::Max(1.f, relativeScale_y);

			if (iSeconds == 60)
			{
				iMinutes += 1;
				iSeconds = 0;
				fInterval = 0;
			}

			fInterval++;
			iSeconds = int(fInterval / 110.f);

			//FPS 
			{
				//render a window
				ImGui::Begin("FPS", NULL, UI_window);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "FPS: %d", cFPSCounter->GetFrameRate());
				ImGui::SetWindowPos(ImVec2(0, 0));
				ImGui::End();
			}

			ImGui::SetNextWindowPos(ImVec2(0, cSettings->iWindowHeight - 100.0f * relativeScale_y));
			ImGui::SetNextWindowSize(ImVec2(100 * relativeScale_x, 30 * relativeScale_y));
			ImGui::Begin("LivesWindow", NULL, UI_window);
			ImGui::SetCursorPosY(-15 * relativeScale_y);
			ImGui::Image((void*)(intptr_t)cTextureManager->MapOfTextureIDs.at(OBJECT_TYPE::INTERACTABLE_LIVES),
				ImVec2(40 * relativeScale_x, 40 * relativeScale_y),
				ImVec2(0, 1), ImVec2(1, 0));
			ImGui::SameLine();
			ImGui::SetCursorPosY(10);
			std::stringstream ss;
			ss.str("");
			ss << cPlayer->iLives << " / " << cPlayer->iMaxLives;
			ImGui::SetWindowFontScale(1.f * relativeScale_y);
			ImGui::TextColored(ImVec4(1, 1, 0, 1), ss.str().c_str());
			ImGui::End();

			ImGui::SetNextWindowFocus();
			//UI bar
			{
				//render a window
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				ImGui::PopStyleColor();
				ImGui::Begin("UI window", NULL, UI_window);

				ImGui::SetWindowPos(ImVec2(-10, cSettings->iWindowHeight * 0.875f));
				ImGui::SetWindowSize(ImVec2(1200.0f * relativeScale_x, 200.0f * relativeScale_y));
				//ImGui::ImageButton((ImTextureID)background.textureID, ImVec2(cSettings->iWindowWidth * 0.7, cSettings->iWindowHeight * 0.7), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0));
				ImGui::Image((void*)(intptr_t)background.textureID, ImVec2(810 * relativeScale_x, 70 * relativeScale_y), ImVec2(0, 1), ImVec2(1, 0));
				ImGui::End();

			}

			//render inventory
			{
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));  // Set a background color
				ImGuiWindowFlags inventoryWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
					ImGuiWindowFlags_NoTitleBar |
					ImGuiWindowFlags_NoMove |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoScrollbar;
				ImGui::PopStyleColor();
				//looping to render the invetory cgui
				for (unsigned int i = 0; i < cPlayerInventory->GetNumofUniqueItems(); i++)
				{
					ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(.0f, .0f, .0f, 1.0f));
					if (cPlayerInventory->iCurrentIndex == i)
					{
						ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.f, 0.f, 0.f, 1.f));
						//ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
					}

					std::stringstream title;
					title.str("");
					title << "Inventory" << i;
					ImGui::SetNextWindowFocus();
					ImGui::Begin(title.str().c_str(), NULL, inventoryWindowFlags);
					ImGui::SameLine();
					ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * (0.56f + i * 0.15), cSettings->iWindowHeight * 0.92f));


					ImGui::SetWindowSize(ImVec2(200.0f * cSettings->GetWindowSize().x, 25.0f * cSettings->GetWindowSize().y));
					ImGui::Image((void*)(intptr_t)cTextureManager->MapOfTextureIDs.at(cPlayerInventory->GetItem(i).get_ID()),
						ImVec2(25 * relativeScale_x, 20 * relativeScale_y),
						ImVec2(0, 1), ImVec2(1, 0));
					ImGui::SameLine();
					ImGui::SetWindowFontScale(1.f * relativeScale_y);
					std::stringstream ss;
					ss.str("");
					ss << ": %d";
					ImGui::TextColored(ImVec4(1, 1, 0, 1), ss.str().c_str(), cPlayerInventory->GetItem(i));
					ImGui::End();
					ImGui::PopStyleColor();
					if (cPlayerInventory->iCurrentIndex == i)
					{
						ImGui::PopStyleColor();
					}

				}
			}
			ImGui::SetNextWindowFocus();
			//render player health
			{
				if (cPlayer->GetHealth() < 3)
				{
					iInterval2 -= 1;
					iTimer2 = iInterval2 / 2;
					if ((iTimer2 / 4) % 2 != 0)
					{
						displayHP = Math::Lerp(displayHP, cPlayer->GetHealth(), 0.2f);
						ImGui::Begin("Health", NULL, health_window);
						ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.02, cSettings->iWindowHeight * 0.9));
						ImGui::SetWindowSize(ImVec2(200.0f * relativeScale_x, 25.0f * relativeScale_y));
						ImGui::SameLine();

						//i think this looks better 
						ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
						ImGui::SetWindowFontScale(1.3f * relativeScale_y);
						ImGui::Text("HP:     ");
						ImGui::SameLine();
						ImGui::SetWindowFontScale(.9f * relativeScale_y);
						ImGui::ProgressBar(displayHP / (float)cPlayer->GetMaxHealth(), ImVec2(cSettings->iWindowWidth * 0.4f, cSettings->iWindowHeight * 0.03f));
						ImGui::PopStyleColor();
						ImGui::PopStyleColor();
						ImGui::End();
					}
				}
				else
				{
					iInterval2 = 300;
					iTimer2 = 0;
					displayHP = Math::Lerp(displayHP, cPlayer->GetHealth(), 0.2f);
					ImGui::Begin("Health", NULL, health_window);
					ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.02, cSettings->iWindowHeight * 0.9));
					ImGui::SetWindowSize(ImVec2(200.0f * relativeScale_x, 25.0f * relativeScale_y));
					ImGui::SameLine();

					//i think this looks better 
					ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
					ImGui::SetWindowFontScale(1.3f * relativeScale_y);
					ImGui::Text("HP:     ");
					ImGui::SameLine();
					ImGui::SetWindowFontScale(.9f * relativeScale_y);
					ImGui::ProgressBar(displayHP / (float)cPlayer->GetMaxHealth(), ImVec2(cSettings->iWindowWidth * 0.4f, cSettings->iWindowHeight * 0.03f));
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::End();
				}
			}
			ImGui::SetNextWindowFocus();
			//render paradoxium timer
			{
				displayTimer = Math::Lerp(displayTimer, cScene2D->GetParadoxiumTimer(), 0.2f);
				ImGui::Begin("Paradoxium Timer", NULL, health_window);
				ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.02, cSettings->iWindowHeight * 0.94));
				ImGui::SetWindowSize(ImVec2(200.0f * relativeScale_x, 25.0f * relativeScale_y));
				ImGui::SameLine();

				//i think this looks better 
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 0.0f, 0.8f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
				ImGui::SetWindowFontScale(1.3f * relativeScale_y);
				ImGui::Text("Energy: ");
				ImGui::SameLine();
				ImGui::SetWindowFontScale(.9f * relativeScale_y);
				ImGui::ProgressBar(displayTimer / (double)cScene2D->GetMaxParadoxiumTimer(), ImVec2(cSettings->iWindowWidth * 0.4f, cSettings->iWindowHeight * 0.03f));

				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::End();
			}
			//render enemy UI
			{
				std::vector<CEnemy2D*> enemy;
				enemy = cEntityManager->GetAllEnemies();
				for (unsigned i = 0; i < enemy.size(); i++)
				{
					float vEnemyPosX = enemy[i]->GetTransformX();
					float vCameraposX = cCamera->GetPosX();
					float finalPosX = vEnemyPosX - vCameraposX;
					finalPosX = finalPosX / cSettings->NUM_TILES_XAXIS * cSettings->iWindowWidth * Camera2D::GetInstance()->getZoom();
					finalPosX += (0.5f * (float)cSettings->iWindowWidth - 25 / Camera2D::GetInstance()->getZoom());

					float vEnemyPosY = enemy[i]->GetTransformY();
					float vCameraposY = cCamera->GetPosY();
					float finalPosY = vEnemyPosY - vCameraposY;
					finalPosY = finalPosY / cSettings->NUM_TILES_YAXIS * cSettings->iWindowHeight * Camera2D::GetInstance()->getZoom();
					finalPosY += 0.5f * (float)cSettings->iWindowHeight;
					finalPosY = cSettings->iWindowHeight - finalPosY - 60;

					if (finalPosX > 0 && finalPosX < cSettings->iWindowWidth &&
						finalPosY > 0 && finalPosY < cSettings->iWindowHeight)
					{
						std::stringstream enemyHealth;
						enemyHealth.str("");
						enemyHealth << "Enemy Health" << i;
						ImGui::Begin(enemyHealth.str().c_str(), NULL, enemyHealth_window);
						ImGui::SetWindowPos(ImVec2(finalPosX, finalPosY));
						ImGui::SetWindowSize(ImVec2(200.0f * relativeScale_x, 25.0f * relativeScale_y));
						ImGui::SameLine();
						ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
						ImGui::ProgressBar((float)enemy[i]->GetHealth() / (float)enemy[i]->GetMaxHealth(), ImVec2(50.0f, 20.0f));
						ImGui::PopStyleColor();
						ImGui::PopStyleColor();
						ImGui::End();
					}
				}
			}
			//render clone health
			{
				//render clone health
				std::vector<CPlayer2D*> clone;
				clone = cEntityManager->GetAllPlayers();

				for (unsigned i = 1; i < clone.size(); i++)
				{
					float vClonePosX = clone[i]->GetTransformX();
					float vCameraposX = cCamera->GetPosX();
					float finalPosX = vClonePosX - vCameraposX;
					finalPosX = finalPosX / cSettings->NUM_TILES_XAXIS * cSettings->iWindowWidth * Camera2D::GetInstance()->getZoom();
					finalPosX += (0.5f * (float)cSettings->iWindowWidth - 25 / Camera2D::GetInstance()->getZoom());

					float vClonePosY = clone[i]->GetTransformY();
					float vCameraposY = cCamera->GetPosY();
					float finalPosY = vClonePosY - vCameraposY;
					finalPosY = finalPosY / cSettings->NUM_TILES_YAXIS * cSettings->iWindowHeight * Camera2D::GetInstance()->getZoom();
					finalPosY += 0.5f * (float)cSettings->iWindowHeight;
					finalPosY = cSettings->iWindowHeight - finalPosY - 60;

					if (finalPosX > 0 && finalPosX < cSettings->iWindowWidth &&
						finalPosY > 0 && finalPosY < cSettings->iWindowHeight)
					{
						std::stringstream enemyHealth;
						enemyHealth.str("");
						enemyHealth << "Clone Health" << i;
						ImGui::Begin(enemyHealth.str().c_str(), NULL, cloneHealth_window);
						ImGui::SetWindowPos(ImVec2(finalPosX, finalPosY));
						ImGui::SetWindowSize(ImVec2(200.0f * relativeScale_x, 25.0f * relativeScale_y));
						ImGui::SameLine();
						ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
						ImGui::ProgressBar((float)clone[i]->GetHealth() / (float)clone[i]->GetMaxHealth(), ImVec2(50.0f, 20.0f));
						ImGui::PopStyleColor();
						ImGui::PopStyleColor();
						ImGui::End();
					}
				}
			}

			return true;
		}
	}
	

	return true;
}

void CPlayGameState::SetLevel(std::string levelPath)
{
	this->levelPath = levelPath;
}
