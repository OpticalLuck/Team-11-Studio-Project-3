#include "InventoryManager.h"
#include "Math/MyMath.h"
CInventoryManager::CInventoryManager()
	:cActiveInventory(NULL)
{
}

CInventoryManager::~CInventoryManager()
{
	m_Inventory.clear();
}

void CInventoryManager::Add(std::string sName)
{
	if (m_Inventory.find(sName) == m_Inventory.end()) //if the inventory doesn't exist 
	{
		CInventory newInventory = CInventory(sName);
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
		std::cout << "Inventory does not exist\n";
}

CInventory* CInventoryManager::Get(std::string sName)
{
	if (m_Inventory.find(sName) != m_Inventory.end()) //if the inventory exist 
	{
		return &m_Inventory.at(sName);
	}
	else
	{
		std::cout << "Inventory does not exist\n";
		return nullptr;
	}
}

void CInventoryManager::NavigateIndex(std::string direction)
{
	if (direction == "DOWN")
	{
		cActiveInventory->iCurrentIndex++;
	}
	if (direction == "UP")
	{
		cActiveInventory->iCurrentIndex--;
	}
	cActiveInventory->iCurrentIndex = Math::Clamp(cActiveInventory->iCurrentIndex, 0, 2);
}
void CInventoryManager::UseItem(void)
{
	cActiveInventory->UseItem();
}

