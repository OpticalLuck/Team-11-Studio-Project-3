/**
 CGUI_Scene2D
 By: Toh Da Jun
 Date: May 2021
 */
#include "GUI_Scene2D.h"
#include "../TextureManager/TextureManager.h"

#include <iostream>
#include <sstream>
using namespace std;

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CGUI_Scene2D::CGUI_Scene2D(void)
	: cSettings(NULL)
	, m_fProgressBar(0.0f)
{
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CGUI_Scene2D::~CGUI_Scene2D(void)
{
	//if (cInventoryManager)
	//{
	//	cInventoryManager->Destroy();
	//	cInventoryManager = NULL;
	//}

	// Show the mouse pointer
	if (cSettings->bDisableMousePointer == true)
		glfwSetInputMode(cSettings->pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		// Hide the cursor
		if (cSettings->bShowMousePointer == false)
			glfwSetInputMode(cSettings->pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// We won't delete this since it was created elsewhere
	cSettings = NULL;
}

/**
  @brief Initialise this instance
  */
bool CGUI_Scene2D::Init(void)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Store the CFPSCounter singleton instance here
	cFPSCounter = CFPSCounter::GetInstance();



	// Define the window flags
	window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	//// Show the mouse pointer
	//glfwSetInputMode(CSettings::GetInstance()->pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	cPlayerInventory = CInventoryManager::GetInstance()->Get("Player");

	
	//cInventoryMain = CInventoryMain::GetInstance();
	cTextureManager = CTextureManager::GetInstance();
	cTextureManager->Init();


	

	//// Add a Tree as one of the inventory items
	//cInventoryItem = cInventoryManager->Add("Shuriken", "Image/Collectibles/shuriken.png", 999, 0);
	//cInventoryItem->vec2Size = glm::vec2(25, 25);
	//cInventoryItem = cInventoryManager->Add("Potion", "Image/items/potion.png", 2, 0);
	//cInventoryItem->vec2Size = glm::vec2(25, 25);
	//cInventoryItem = cInventoryManager->Add("Hook", "Image/items/grappling_hook.png", 2, 0);
	//cInventoryItem->vec2Size = glm::vec2(25, 25);

	// Add a Lives icon as one of the inventory items
	// cInventoryItem = cInventoryManager->Add("Lives", "Image/Collectibles/Scene2D_Lives.tga", 5, 3);
	// cInventoryItem->vec2Size = glm::vec2(25, 25);

	// Add a Health icon as one of the inventory items
	// cInventoryItem = cInventoryManager->Add("Health", "Image/Scene2D_Health.tga", 100, 100);
	// cInventoryItem->vec2Size = glm::vec2(25, 25);

	// Add a Tree as one of the inventory items
	// cInventoryItem = cInventoryManager->Add("Shuriken", "Image/Collectibles/shuriken.png", 999, 5);
	// cInventoryItem->vec2Size = glm::vec2(25, 25);
	// cInventoryItem = cInventoryManager->Add("Potion", "Image/items/potion.png", 2, 0);
	// cInventoryItem->vec2Size = glm::vec2(25, 25);
	// cInventoryItem = cInventoryManager->Add("Hook", "Image/items/grappling_hook.png", 2, 0);
	// cInventoryItem->vec2Size = glm::vec2(25, 25);


	fInterval = 0;
	iMinutes = 0;
	iSeconds = 0;


	return true;
}

/**
 @brief Update this instance
 */
void CGUI_Scene2D::Update(const double dElapsedTime)
{
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
	/*ImGui::TextColored(ImVec4(1, 1, 0, 1), "FPS: %d", cFPSCounter->GetFrameRate());*/
	ImGui::TextColored(ImVec4(1, 1, 1, 1),"Timer = %d : %d",iMinutes,iSeconds);

	// Render the inventory items
	
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
		}

		std::stringstream title;
		title.str("");
		title << "Inventory" << i;
		ImGui::Begin(title.str().c_str(), NULL, inventoryWindowFlags);
		ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.01f, cSettings->iWindowHeight * (0.065f * i + 0.05)));
		ImGui::SetWindowSize(ImVec2(200.0f * relativeScale_x, 25.0f * relativeScale_y));
		ImGui::Image((void*)(intptr_t)cTextureManager->MapOfTextureIDs.at(cPlayerInventory->GetItem(i).get_ID()),
			ImVec2(25 * relativeScale_x, 25 * relativeScale_y),
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
	ImGui::End();
}


/**
 @brief Set up the OpenGL display environment before rendering
 */
void CGUI_Scene2D::PreRender(void)
{
}

/**
 @brief Render this instance
 */
void CGUI_Scene2D::Render(void)
{
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CGUI_Scene2D::PostRender(void)
{
}
