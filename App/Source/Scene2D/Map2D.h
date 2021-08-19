/**
 Map2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include SingletonTemplate
#include "DesignPatterns\SingletonTemplate.h"

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include the RapidCSV
#include "System/rapidcsv.h"
// Include map storage
#include <map>

// Include Settings
#include "GameControl\Settings.h"

// Include Entity2D
#include "Primitives/Entity2D.h"

//Include CObject2D
#include "../Factory/ObjectFactory.h"

//Include player
#include "Player2D.h"

//Include camera
#include "Camera2D.h"

#include "../TextureManager/TextureManager.h"

// Include files for AStar
#include <queue>
#include <functional>

class CMap2D : public CSingletonTemplate<CMap2D>, public CEntity2D
{
	friend CSingletonTemplate<CMap2D>;
public:
	// Init
	bool Init(	const unsigned int uiNumLevels = 1,
				const unsigned int uiNumRows = 24,
				const unsigned int uiNumCols = 32);

	// Update
	void Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

	// Set the value at certain indices in the arrMapInfo - Doesnt care if there is something there since it will replace
	void SetMapInfo(unsigned int uiRow, unsigned int uiCol, const int iValue, const bool bInvert = true);

	// Insert mapinfo into the gridarr and arrobj, will return false if something is in the way
	bool InsertMapInfo(unsigned int uiRow, unsigned int uiCol, const int iValue, const bool bInvert = true);
	
	//Replace with existing cobject
	void ReplaceGridInfo(const unsigned int uiRow, const unsigned uiCol, CObject2D* target, const bool bInvert = true);

	std::vector<CObject2D*> GetObjectArr();

	CObject2D* GetCObject(const unsigned int uiCol, const unsigned int uiRow, const bool bInvert = true);

	// Get the value at certain indices in the arrMapInfo
	int GetMapInfo(const unsigned int uiRow, const unsigned int uiCol, const bool bInvert = true) const;

	// Load a map
	bool LoadMap(string filename, const unsigned int uiLevel = 0);

	// Save a tilemap
	bool SaveMap(string filename, const unsigned int uiLevel = 0);

	// Find the indices of a certain value in arrMapInfo
	bool FindValue(const int iValue, unsigned int& uirRow, unsigned int& uirCol);

	// Set current level
	void SetCurrentLevel(unsigned int uiCurLevel);
	// Get current level
	unsigned int GetCurrentLevel(void) const;

	//Get current level max rows and cols
	int GetLevelRow(void);
	int GetLevelCol(void);

	//Get arrlevellimit
	glm::i32vec2 GetLevelLimit(void);

protected:
	// The variable containing the rapidcsv::Document
	// We will load the CSV file's content into this Document
	rapidcsv::Document doc;

	ObjectFactory objFactory;

	// Vector array that stores an array of objects level by level
	std::vector<std::vector<CObject2D*>> arrObject;

	//2D array
	std::vector<std::vector<std::vector<CObject2D*>>> arrGrid; //Y, X

	//Vector array that stores limits of each level
	std::vector<glm::i32vec2> arrLevelLimit;

	CTextureManager* cTextureManager;

	// The current level
	unsigned int uiCurLevel;
	// The number of levels
	unsigned int uiNumLevels;

	//CS: The quadMesh for drawing the tiles
	CMesh* quadMesh;

	//Camera handler
	Camera2D* camera;

	// Constructor
	CMap2D(void);

	// Destructor
	virtual ~CMap2D(void);

	// Render a tile
	void RenderTile(const CObject2D obj);
};

