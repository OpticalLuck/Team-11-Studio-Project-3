#pragma once

// Include Singleton Pattern
#include "DesignPatterns/SingletonTemplate.h"

// Include Filesystem
#include "System\filesystem.h"

// Include the RapidCSV
#include "System/rapidcsv.h"

#include <string>

#include <vector>

struct sCell
{
	int iTileID = 0;
};

struct Level
{
	string LevelName;
	string LevelPath;
};

class CLevelEditor : CSingletonTemplate<CLevelEditor>
{
	friend CSingletonTemplate<CLevelEditor>;
public:

	unsigned int iWorldWidth;
	unsigned int iWorldHeight;

	void Init(unsigned int width, unsigned int height);
	void LoadLevel(const char* filePath);
	void GetExistingLevels(void);
	void UpdateCell(unsigned int x, unsigned int y, int TileID);

protected:

	rapidcsv::Document doc;

	std::vector<Level> m_Levels;
	sCell* m_Map;

	CLevelEditor();
	~CLevelEditor();

};

