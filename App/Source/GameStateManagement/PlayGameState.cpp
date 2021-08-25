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
	, iMinutes(0.f)
	, iSeconds(0.f)
	, transformX(0.f)
	, transformY(0.f)
	, cCamera(NULL)
	, cSoundController(NULL)
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
	if (cScene2D->Init() == false)
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

	cGameStateManager = CGameStateManager::GetInstance();

	// Create and initialise the Map 2D
	cMap2D = CMap2D::GetInstance();
	cCamera = Camera2D::GetInstance();

	cSoundController = CSoundController::GetInstance();

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
	if (cPlayer->GetHealth() < 3)
	{
		cSoundController->PlaySoundByID(SOUND_ID::SOUND_TROUBLE);
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
	cout << "CPlayGameState::Destroy()\n" << endl;

	// Destroy the cScene2D instance
	if (cScene2D)
	{
		cScene2D->Destroy();
		cScene2D = NULL;
	}
	if (cInputHandler)
	{
		cInputHandler->Destroy();
		cInputHandler = NULL;
	}
}

bool CPlayGameState::ImGuiRender()
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

		// Create an invisible window which covers the entire OpenGL window
		ImGui::Begin("Invisible window", NULL, window_flags);
		ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
		ImGui::SetWindowFontScale(1.5f * relativeScale_y);
		// Display the FPS
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "FPS: %d", cFPSCounter->GetFrameRate());
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "Timer = %d : %d", iMinutes, iSeconds);

		ImGui::SameLine();
		ImGui::InvisibleButton("temp", ImVec2(50, 1));


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
			for (int i = 0; i < cPlayerInventory->GetNumofUniqueItems(); i++)
			{
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
				if (cPlayerInventory->iCurrentIndex == i)
				{
					ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.f, 0.f, 0.f, 1.f));
					//ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
				}

				std::stringstream title;
				title.str("");
				title << "Inventory" << i;
				ImGui::Begin(title.str().c_str(), NULL, inventoryWindowFlags);
				ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.01f, cSettings->iWindowHeight * (0.065f * i + 0.05)));
				ImGui::SetWindowSize(ImVec2(200.0f * relativeScale_x, 25.0f * relativeScale_y));
				ImGui::Image((void*)(intptr_t)cTextureManager->MapOfTextureIDs.at(cPlayerInventory->GetItem(i).get_ID()),
					ImVec2(25 * relativeScale_x, 15 * relativeScale_y),
					ImVec2(0, 1), ImVec2(1, 0));
				ImGui::SameLine();
				ImGui::SetWindowFontScale(1.5f * relativeScale_y);
				std::stringstream ss;
				ss.str("");
				ss << cPlayerInventory->GetItem(i).GetName() << ": %d";
				ImGui::TextColored(ImVec4(1, 1, 0, 1), ss.str().c_str(), cPlayerInventory->GetItem(i));
				ImGui::End();
				ImGui::PopStyleColor();
				if (cPlayerInventory->iCurrentIndex == i)
				{
					ImGui::PopStyleColor();
				}

			}
		}
		//render player health
		{	
			float vPlayerPosX = cPlayer->GetTransformX();
			float vCameraposX = cCamera->GetPosX();
			float finalPosX = vPlayerPosX - vCameraposX;
			finalPosX = finalPosX / cSettings->NUM_TILES_XAXIS * cSettings->iWindowWidth;
			finalPosX += 0.5 * cSettings->iWindowWidth - 25;

			float vPlayerPosY = cPlayer->GetTransformY();
			float vCameraposY = cCamera->GetPosY();
			float finalPosY = vPlayerPosY - vCameraposY;
			finalPosY = finalPosY / cSettings->NUM_TILES_YAXIS * cSettings->iWindowHeight;
			finalPosY += 0.5 * cSettings->iWindowHeight;
			finalPosY = cSettings->iWindowHeight - finalPosY;


			ImGui::Begin("Health", NULL, health_window);
			ImGui::SetWindowPos(ImVec2(finalPosX, finalPosY));
			ImGui::SetWindowSize(ImVec2(200.0f * relativeScale_x, 25.0f * relativeScale_y));
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.6f, 0.196f, 0.8f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.f, 1.f, 0.f, 1.0f));
			ImGui::ProgressBar(cPlayer->GetHealth() / (float)cPlayer->GetMaxHealth(), ImVec2(50.0f, 20.0f));
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
				finalPosX = finalPosX / cSettings->NUM_TILES_XAXIS * cSettings->iWindowWidth;
				finalPosX += 0.5 * cSettings->iWindowWidth - 20;

				float vEnemyPosY = enemy[i]->GetTransformY();
				float vCameraposY = cCamera->GetPosY();
				float finalPosY = vEnemyPosY - vCameraposY;
				finalPosY = finalPosY / cSettings->NUM_TILES_YAXIS * cSettings->iWindowHeight;
				finalPosY += 0.5 * cSettings->iWindowHeight;
				finalPosY = cSettings->iWindowHeight - finalPosY - 60;

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
				finalPosX = finalPosX / cSettings->NUM_TILES_XAXIS * cSettings->iWindowWidth;
				finalPosX += 0.5 * cSettings->iWindowWidth - 20;

				float vClonePosY = clone[i]->GetTransformY();
				float vCameraposY = cCamera->GetPosY();
				float finalPosY = vClonePosY - vCameraposY;
				finalPosY = finalPosY / cSettings->NUM_TILES_YAXIS * cSettings->iWindowHeight;
				finalPosY += 0.5 * cSettings->iWindowHeight;
				finalPosY = cSettings->iWindowHeight - finalPosY - 60;

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
		ImGui::End();

		return true;
	}
}
