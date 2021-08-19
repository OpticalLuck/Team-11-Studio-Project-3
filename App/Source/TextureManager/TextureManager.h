#pragma once
#include "DesignPatterns/SingletonTemplate.h"
#include <map>

class CTextureManager : public CSingletonTemplate<CTextureManager>
{
	friend CSingletonTemplate<CTextureManager>;
public:

	enum TILE_TYPE
	{
		TILE_GROUND = 100,
		TILE_WALL_1 = 101,
		TILE_WALL_2 = 102,
		TILE_GRASS = 103,
		TILE_LEFT_GRASS = 104,
		TILE_RIGHT_GRASS = 105,
		TILE_BOULDER = 150,
		TILE_TOTAL,
	};

	// Map containing texture IDs
	std::map<int, int> MapOfTextureIDs;

	bool Init(void);

protected:

	CTextureManager();
	~CTextureManager();

	bool LoadTexture(const char* filename, const int iTextureCode);

};

