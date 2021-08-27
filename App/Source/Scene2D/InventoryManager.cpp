#include "InventoryManager.h"
#include "Math/MyMath.h"
#include "System/Debug.h"
CInventoryManager::CInventoryManager()
	:cActiveInventory(NULL)
{
}

CInventoryManager::~CInventoryManager()
{
	m_Inventory.clear();
}

void CInventoryManager::Add(std::string sName, CPlayer2D* target)
{
	DEBUG_MSG(sName);

	if (m_Inventory.find(sName) == m_Inventory.end()) //if the inventory doesn't exist 
	{
		CInventory newInventory = CInventory(sName, target);
		newInventory.Init();
		std::pair<std::string, CInventory> pairInventory = std::pair<std::string, CInventory>(sName, newInventory);
		m_Inventory.insert(pairInventory);
	}
}

void CInventoryManager::Use(std::string sName)
{
	if (m_Inventory.find(sName) != m_Inventory.end()) //if the inventory exist 
	{
		cActiveInventory = &m_Inventory.at(sName);
	}
	else
		DEBUG_MSG("Inventory does not exist");
}

CInventory* CInventoryManager::Get(std::string sName)
{
	if (m_Inventory.find(sName) != m_Inventory.end()) //if the inventory exist 
	{
		return &m_Inventory.at(sName);
	}
	else
	{
		DEBUG_MSG("Inventory does not exist\n");
		return nullptr;
	}
}

void CInventoryManager::DeleteInventory(std::string sName)
{
	if (m_Inventory.find(sName) != m_Inventory.end()) //if the inventory exist 
	{
		m_Inventory.erase(sName);
	}
}

void CInventoryManager::NavigateIndex(std::string direction)
{
	if (direction == "RIGHT")
	{
		cActiveInventory->iCurrentIndex++;
	}
	if (direction == "LEFT")
	{
		cActiveInventory->iCurrentIndex--;
	}
	cActiveInventory->iCurrentIndex = Math::Clamp(cActiveInventory->iCurrentIndex, 0, 1);
}
void CInventoryManager::UseItem(void)
{
	cActiveInventory->UseItem();
}

