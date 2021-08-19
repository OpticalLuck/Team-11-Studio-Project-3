#pragma once

// Include CEntity2D
#include "Primitives/Entity2D.h"
#include "InventoryManager.h"

#include <map>
#include <string>

class CInventoryMain
{

public:

	// Constructor
	CInventoryMain(void);

	// Destructor
	~CInventoryMain(void);

	bool Init(void);

	void AddItem(string item);

	void GetItem(string item);

	int GetItemIndex(void);

	void NavigateIndex(string direction);

	unsigned int GetTexture(void) const;

	int GetItemCount(void);

	glm::vec2 GetItemSize(void);
	int iItemIndex;

private:

	CInventoryItem* cInventoryItem;
	CInventoryManager* cInventoryManager;


};
