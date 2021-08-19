#pragma once
#include "Item.h"

#include <map>
#include <vector>

class CInventory
{
public:

	CInventory();
	~CInventory();

	void AddItem(int iCount,int iID);

	


private:
    std::map<int, int> m_ItemCount;

};