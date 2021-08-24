#pragma once
#include "DesignPatterns/SingletonTemplate.h"

#include <map>

class CTextureManager : public CSingletonTemplate<CTextureManager>
{
	friend CSingletonTemplate<CTextureManager>;
public:

	// Map containing texture IDs
	std::map<int, int> MapOfTextureIDs;

	bool Init(void);

protected:

	CTextureManager();
	~CTextureManager();

	bool LoadTexture(const char* filename, const int iTextureCode);

};

