/**
 Map2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include SingletonTemplate
#include "../Library/Source/DesignPatterns/SingletonTemplate.h"

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
#include "../Library/Source/System/rapidcsv.h"
// Include map storage
#include <map>

// Include Settings
class CSettings;

// Include Entity2D
#include "../Library/Source/Primitives/Entity2D.h"

//Include CObject2D
#include "../Factory/ObjectFactory.h"

//Include player
#include "Player2D.h"

//Include camera
#include "Camera2D.h"

//Include background entity
#include "BackgroundEntity.h"

//Include texture manager
#include "../TextureManager/TextureManager.h"

// Include files for AStar
#include <queue>
#include <functional>

//Include stringstream for extraction of data
#include <sstream>

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

	//Render background
	void RenderBackground(void);

	// Set the value at certain indices in the arrMapInfo - Doesnt care if there is something there since it will replace
	void SetMapInfo(unsigned int uiRow, unsigned int uiCol, const int iTextureID, const bool bInvert = true);

	// Insert mapinfo into the gridarr and arrobj, will return false if something is in the way
	bool InsertMapInfo(unsigned int uiRow, unsigned int uiCol, const int iTextureID, const bool bInvert = true);
	
	//Update object's grid location with new row and col
	void UpdateGridInfo(const unsigned int uiRow, const unsigned uiCol, CObject2D* target, const bool bInvert = true);

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
	std::vector< std::vector<CObject2D*>> arrBgObject;

	//2D array
	std::vector<std::vector<std::vector<CObject2D*>>> arrGrid; //Y, X
	std::vector<std::vector<std::vector<CObject2D*>>> arrBgGrid; //Y, X

	//Vector array that stores limits of each level
	std::vector<glm::i32vec2> arrLevelLimit;


	//Vector arrays storing BackgroundEntity and allowance scale
	std::vector<CBackgroundEntity*> arrBackground;
	std::vector<glm::vec2> arrAllowanceScale;

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
	void RenderTile(const CObject2D& obj);
};

