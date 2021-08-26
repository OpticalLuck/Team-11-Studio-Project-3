// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif


#include "Inventory.h"
#include "InventoryManager.h"
#include "Math/MyMath.h"
#include "GLFW/glfw3.h"
#include "Object2D.h"
#include "Map2D.h"
#include "System/Debug.h"

CItem CInventory::itemDef = CItem();

CInventory::CInventory(std::string sName, CPlayer2D* target)
	: fCooldown(0.f)
	, iCurrentIndex(0)
{
	this->sName = sName;
	linkedPlayer = target;
}


CInventory::~CInventory()
{
}

void CInventory::AddItem(std::string itemName, int iCount)
{
	// If the item exist in the inventory
	for (unsigned i = 0; i < m_Items.size(); ++i)
	{
		if (m_Items[i].GetName() == itemName)
		{
			CItem& currentItem = m_Items[i];
			currentItem.iCount += iCount;
			currentItem.iCount = Math::Clamp(currentItem.iCount, currentItem.iMinCount, currentItem.iMaxCount);
			return;
		}
	}

	DEBUG_MSG("Item is not in the inventory");

}

void CInventory::Update(double dElapsedTime, int iTempFrameCounter, std::vector<std::array<KeyInput, KEYBOARD_INPUTS::KEY_TOTAL>> m_KeyboardInputs, std::vector<std::array<MouseInput, MOUSE_INPUTS::MOUSE_TOTAL>> m_MouseInputs)
{
	CInventoryManager* cInventoryManager = CInventoryManager::GetInstance();
	cInventoryManager->Use(sName);

	if (fCooldown > 0)
	{
		fCooldown -= (float)dElapsedTime;
	}
	if (m_KeyboardInputs[iTempFrameCounter][KEYBOARD_INPUTS::ARROW_UP].bKeyPressed && fCooldown <= 0)
	{
		cInventoryManager->NavigateIndex("UP");
		std::cout << "index is : " << iCurrentIndex << std::endl;
		fCooldown = .5f;
	}
	if (m_KeyboardInputs[iTempFrameCounter][KEYBOARD_INPUTS::ARROW_DOWN].bKeyPressed && fCooldown <= 0)
	{
		cInventoryManager->NavigateIndex("DOWN");
		std::cout << "index is : " << iCurrentIndex << std::endl;
		fCooldown = .5f;
	}

	//if (cKeyboardController->IsKeyPressed(GLFW_KEY_G))
	//{
	//	//cInventoryM->AddItem("Shuriken", ITEM_SHURIKEN);
	//}
}

void CInventory::Init()
{
	//m_Items.insert(pair<int, CItem>(0,CItem(2)));

	SetItem(0, "Shuriken", PROJECTILES_SHURIKEN, 0, 100, 10);
	SetItem(1, "Kunai", BULLETS_KUNAI, 0, 100, 10);
	SetItem(2, "Potion", CONSUMABLES_POTION, 0, 100, 2);
}

void CInventory::SetItem(int iIndex, std::string sName, int iID, int iMinVal, int iMaxVal, int iSetVal)
{
	if (m_Items.find(iIndex) == m_Items.end()) // if doesnt exist
	{
		CItem newItem = CItem(iID);
		newItem.iMinCount = iMinVal;
		newItem.iMaxCount = iMaxVal;
		newItem.iCount = iSetVal;
		newItem.set_Name(sName);

		std::pair<int, CItem> pairItem = std::pair<int, CItem>(iIndex,newItem);
		m_Items.insert(pairItem);
	}
	else
	{
		cout << "Slot has been occupied\n";
	}

}

CItem& CInventory::GetItem(int iIndex)
{
	if (m_Items.find(iIndex) != m_Items.end()) //if item is existed
	{
		return m_Items.at(iIndex);
	}
	else
		cout << "Item does not exist\n";

	return itemDef;
}

int CInventory::GetNumofUniqueItems()
{
	return m_Items.size();
}

void CInventory::UseItem(void)
{
	if (m_Items.find(iCurrentIndex) != m_Items.end()) //if item is existed
	{
		m_Items.at(iCurrentIndex).Use(linkedPlayer);
	}
	else
		cout << "Item does not exist\n";
}

void CInventory::SetPlayer(CPlayer2D* player)
{
	linkedPlayer = player;
}
