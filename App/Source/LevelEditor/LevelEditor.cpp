#include "LevelEditor.h"

#include <filesystem>
#include <iostream>

using namespace std;

/**
@brief Initialise this instance
*/
void CLevelEditor::Init(unsigned int width, unsigned int height)
{
	iWorldWidth = width;
	iWorldHeight = height;

	// Create a Dynamic Array with World Width and Height
	m_Map = new sCell[iWorldHeight * iWorldWidth];
}

/**
@brief Loads an existing CSV into the level editor
*/
void CLevelEditor::LoadLevel(const char* filePath)
{
	// Load the Level CSV
	doc = rapidcsv::Document(FileSystem::getPath(filePath).c_str());

	// Set World Width and Height
	iWorldWidth = doc.GetColumnCount();
	iWorldHeight = doc.GetRowCount();

	// Iterate through the Level Editor Map and set the values of the corresponding indexes
	for (int iRow = 0; iRow < iWorldHeight; ++iRow)
	{
		std::vector<std::string> row = doc.GetRow<std::string>(iRow);
		for (int iCol = 0; iCol < iWorldWidth; ++iCol)
		{
			int currVal = (int)stoi(row[iCol]);
			m_Map[iCol + iRow * iWorldWidth].iTileID = currVal;
		}
	}
}

/**
@brief Updates the Cell in the level editor given an x and y index
*/
void CLevelEditor::UpdateCell(unsigned int x, unsigned int y, int TileID)
{
	m_Map[x + y * iWorldWidth].iTileID = TileID;
}

void CLevelEditor::GetExistingLevels(void)
{
	// for (const auto & file : )
}

CLevelEditor::CLevelEditor()
{
}

CLevelEditor::~CLevelEditor()
{
}
