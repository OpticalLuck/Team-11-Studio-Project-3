#include "Inventory.h"

CInventory::CInventory()
{
}

CInventory::~CInventory()
{
}

void CInventory::AddItem(int iCount, int iID)
{
	// If the item does not exist in the inventory
	if (m_ItemCount.find(iID) == m_ItemCount.end())
	{
		std::pair<int, int> itemCount = std::pair<int, int>(iCount, iID);
		m_ItemCount.insert(itemCount);
	}
	else // Item exists
	{
		m_ItemCount[iID] = iCount;
	}
}
