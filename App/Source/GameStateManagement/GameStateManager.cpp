#include "GameStateManager.h"
#include <iostream>

using namespace std;

/**
 @brief Constructor
 */
CGameStateManager::CGameStateManager(void)
	: activeGameState(nullptr)
	, nextGameState(nullptr)
	, prevGameState(nullptr)
	, pauseGameState(nullptr)
	, bPause(false)
	, bOption(false)
{
}

/**
 @brief Destructor
 */
CGameStateManager::~CGameStateManager(void)
{
	Delete();
}

/**
 @brief Destroy this class instance
 */
void CGameStateManager::Delete(void)
{
	// Set the handlers to CGameState to nullptr
	activeGameState = nullptr;
	nextGameState = nullptr;
	prevGameState = nullptr;

	// Delete all scenes stored and empty the entire map
	std::map<std::string, CGameStateBase*>::iterator it, end;
	end = GameStateMap.end();
	for (it = GameStateMap.begin(); it != end; ++it)
	{
		delete it->second;
	}
	// Clear the map
	GameStateMap.clear();

}

/**
 @brief Update this class instance
 */
bool CGameStateManager::Update(const double dElapsedTime)
{
	// Check for change of scene
	if (nextGameState != activeGameState)
	{
		if (activeGameState)
		{
			// Scene is valid, need to call appropriate function to exit
			activeGameState->Destroy();
		}
		//
		// Set the previous CGameState
		prevGameState = activeGameState;
		// Set the new active CGameState
		activeGameState = nextGameState;
		// Init the new active CGameState
 		activeGameState->Init();
	}

	// Update the active CGameState
	if (activeGameState && !pauseGameState)
	{
		if (activeGameState->Update(dElapsedTime) == false)
			return false;
	}

	//Pause initializing is handled in SetPauseGameState
	if (pauseGameState)
	{
		//Update imgui components
		if (pauseGameState->Update(dElapsedTime) == false)
			return false;
		if (!bPause)
		{
			//Destroy pause instance
			pauseGameState->Destroy();
			pauseGameState = nullptr;

			//Update imgui components
			if (activeGameState->Update(dElapsedTime) == false)
				return false;
		}
	}

	return true;
}

/**
 @brief Render this class instance
 */
bool CGameStateManager::Render(void)
{
	if (activeGameState)
	{
		activeGameState->Render();
		if (!activeGameState->ImGuiRender())
		{
			return false;
		}
		if (pauseGameState)
		{
			//pauseGameState->Render();
			if (!pauseGameState->ImGuiRender())
			{
				return false;
			}
		}
	}

	return true;
}

/**
 @brief Add a new CGameState to this
 @param _name A const std::string& variable which is the name of the CGameState
 @param _scene A CGameState* variable which is the CGameState
 */
bool CGameStateManager::AddGameState(const std::string& _name, CGameStateBase* _scene)
{
	// Check if this _name already exists in the map...
	if (CheckGameStateExist(_name))
	{
		// If it does not exist, then unable to proceed
		cout << "CGameStateManager::AddGameState - Duplicate scene name provided" << endl;
		return false;
	}

	// Check if the _scene is valid
	if (_scene == nullptr)
	{
		cout << "CGameStateManager::AddGameState - Invalid scene pointer" << endl;
		return false;
	}

	// If nothing is wrong, then add the scene to our map
	GameStateMap[_name] = _scene;

	return true;
}

/**
 @brief Remove a CGameState from this
 @param _name A const std::string& variable which is the name of the CGameState
 */
bool CGameStateManager::RemoveGameState(const std::string& _name)
{
	// Check if this _name does not exists in the map...
	if (!CheckGameStateExist(_name))
	{
		// If it does not exist, then unable to proceed
		cout << "CGameStateManager::RemoveGameState - scene name does not exists" << endl;
		return false;
	}

	// Find the CGameState*
	CGameStateBase* target = GameStateMap[_name];
	if (target == activeGameState || target == nextGameState)
	{
		cout << "Unable to remove active/next scene" << endl;
		return false;
	}

	// Delete and remove from our map
	delete target;
	GameStateMap.erase(_name);

	return true;
}

/**
 @brief Set a CGameState to be active
 @param _name A const std::string& variable which is the name of the CGameState
 */
bool CGameStateManager::SetActiveGameState(const std::string& _name)
{
	// Check if this _name does not exists in the map...
	if (!CheckGameStateExist(_name))
	{
		// If it does not exist, then unable to proceed
		cout << "CGameStateManager::SetActiveGameState - scene name does not exists" << endl;
		return false;
	}

	// Scene exist, set the next scene pointer to that scene
	nextGameState = GameStateMap[_name];

	return true;
}

bool CGameStateManager::CheckGameStateExist(const std::string& _name)
{
	return GameStateMap.count(_name) != 0;
}

CGameStateBase* CGameStateManager::GetState(const std::string& _name)
{
	return GameStateMap.at(_name);
}

bool CGameStateManager::SetPauseGameState(const std::string& _name)
{
	// Toggle to nullptr if pauseGameState already is in use
	if (pauseGameState != nullptr)
	{
		pauseGameState = nullptr;
		return true;
	}

	// Check if this _name does not exists in the map...
	if (!CheckGameStateExist(_name))
	{
		// If it does not exist, then unable to proceed
		cout << "CGameStateManager::SetPauseGameState - scene name does not exists" << endl;
		return false;
	}

	// Scene exist, set the next scene pointer to that scene
	pauseGameState = GameStateMap[_name];
	// Init the new pause CGameState
	pauseGameState->Init();

	bPause = true;
	return true;
}

void CGameStateManager::OffPauseGameState()
{
	//Cannot delete imgui context inside the gamestate, must be handled after update
	bPause = false;
	bOption = false;
}
