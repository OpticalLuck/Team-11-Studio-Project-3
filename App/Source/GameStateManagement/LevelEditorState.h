#pragma once

/**
 CLevelEditorState
 @brief This class is derived from CGameState. It will introduce the game to the player.
 By: Toh Da Jun
 Date: July 2021
 */

#include "GameStateBase.h"
#include "../LevelEditor/LevelEditor.h"
#include "../LevelEditor/LevelGrid.h"
#include <Inputs/KeyboardController.h>
#include "Inputs/MouseController.h"
#include <deque>

#ifndef IMGUI_ACTIVE
#include "GUI\imgui.h"
#include "GUI\backends\imgui_impl_glfw.h"
#include "GUI\backends\imgui_impl_opengl3.h"
#define IMGUI_ACTIVE
#endif

struct ActionState
{
	std::vector<std::vector<sCell>> m_ThisActionLevel;
	unsigned int iWorldWidth = 32;
	unsigned int iWorldHeight = 24;
};

struct EditorProperties
{
	int iUndoCount = 0;

	bool bPlacedBlock = false;
	bool bDeletedBlock = false;

	bool bSaved = true;
	bool bToggleCloseWindow = false;
	bool bToggleEditorWindow = true;
	bool bToggleHistoryWindow = true;
	bool bHistoryUpdated = false;

	bool bIsSelecting = false;
	glm::vec2 WideAreaSelectionStart = { 0 , 0 };
	glm::vec2 WideAreaSelectionEnd = { 0 , 0 };

	std::deque<ActionState> undoStates;
	std::deque<ActionState> redoStates;

	std::vector<std::string> prevActions;

	void RecalculateStartEndIndex(int& startXIndex, int& endXIndex, int& startYIndex, int& endYIndex)
	{
		if (this->WideAreaSelectionStart.x < this->WideAreaSelectionEnd.x)
		{
			startXIndex = this->WideAreaSelectionStart.x;
			endXIndex = this->WideAreaSelectionEnd.x;
		}
		else
		{
			startXIndex = this->WideAreaSelectionEnd.x;
			endXIndex = this->WideAreaSelectionStart.x;
		}

		if (this->WideAreaSelectionStart.y < this->WideAreaSelectionEnd.y)
		{
			startYIndex = this->WideAreaSelectionStart.y;
			endYIndex = this->WideAreaSelectionEnd.y;
		}
		else
		{
			startYIndex = this->WideAreaSelectionEnd.y;
			endYIndex = this->WideAreaSelectionStart.y;
		}
	}

	void Reset()
	{
		iUndoCount = 0;
		bSaved = true;
		bToggleCloseWindow = false;
		bToggleEditorWindow = true;

		bPlacedBlock = false;
		bDeletedBlock = false;

		bIsSelecting = false;
		WideAreaSelectionStart = { 0 , 0 };
		WideAreaSelectionEnd = { 0 , 0 };

		undoStates.clear();
		redoStates.clear();
		prevActions.clear();
	}
};

class CLevelEditorState : public CGameStateBase
{
public:
	// Constructor
	CLevelEditorState(void);
	// Destructor
	~CLevelEditorState(void);

	// Init this class instance
	virtual bool Init(void);
	// Update this class instance
	virtual bool Update(const double dElapsedTime);
	// Render this class instance
	virtual void Render(void);
	// Destroy this class instance
	virtual void Destroy(void);

protected:

	EditorProperties eProperties;

	glm::i32vec2 vMousePos;
	glm::mat4 transform;

	unsigned FBO, RBO, textureColorBuffer;
	unsigned int quadVAO, quadVBO;

	CMesh* cursor;

	CSettings*					cSettings;

	CKeyboardController* 		cKeyboardController;
	CMouseController*			cMouseController;
	CLevelEditor*				cLevelEditor;
	CLevelGrid*					cLevelGrid;

	int activeTile;

	void MoveCamera(double dElapsedTime);
	void ScaleMap(void);
	void MouseInput(double dElapsedTime);

	void GenerateQuadVAO(void);
	void GenerateFBO(void);
	void RenderQuad(unsigned int iTextureID);
	void ImGuiRender(void);
	void RenderCursor(void);

	bool EditorShortcuts(void);
	bool FileUtilShortcuts(void);
	void AreaFill(void);
	void AreaDelete(void);
	void Undo(int count);
	void Redo(int count);
	void CopyBlock(void);
	void Save();
	void Close();

	void IncreaseXSize(void);
	void DecreaseXSize(void);
	void IncreaseYSize(void);
	void DecreaseYSize(void);

	void UpdateHistory(void);
};
