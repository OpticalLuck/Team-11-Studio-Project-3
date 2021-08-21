// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif


#include "Inventory.h"
#include "InventoryManager.h"
#include "..\KeyboardInputHandler\CKeyboardInputHandler.h"
#include "Math/MyMath.h"
#include "GLFW/glfw3.h"
#include "Object2D.h"

CInventory::CInventory(std::string sName)
	: fCooldown(0.f)
	, iCurrentIndex(0)
{
	this->sName = sName;
}


CInventory::~CInventory()
{
}

void CInventory::AddItem(int iIndex, int iID, int iCount)
{
	// If the item exist in the inventory
	if (m_Items.find(iIndex) != m_Items.end())
	{
		CItem& currentItem = m_Items.at(iIndex);
		currentItem.iCount += iCount;
		currentItem.iCount = Math::Clamp(currentItem.iCount, currentItem.iMinCount , currentItem.iMaxCount);
	}
	else 
		std::cout << "Item is not in inventory\n";

}

void CInventory::Update(double dElapsedTime)
{
	CKeyboardController* cKeyboardController = CKeyboardController::GetInstance();
	CInventoryManager* cInventoryManager = CInventoryManager::GetInstance();
	cInventoryManager->Use(sName);


	if (fCooldown > 0)
	{
		fCooldown -= (float)dElapsedTime;
	}

	if (cKeyboardController->IsKeyPressed(GLFW_KEY_UP) && fCooldown <= 0)
	{
		cInventoryManager->NavigateIndex("UP");
		std::cout << "index is : " << iCurrentIndex << std::endl;
		fCooldown = .5f;
	}
	if (cKeyboardController->IsKeyPressed(GLFW_KEY_DOWN) && fCooldown <= 0)
	{
		cInventoryManager->NavigateIndex("DOWN");
		std::cout << "index is : " << iCurrentIndex << std::endl;
		fCooldown = .5f;
	}

	if (cKeyboardController->IsKeyPressed(GLFW_KEY_ENTER) && fCooldown <= 0)
	{
		cInventoryManager->UseItem();
		fCooldown = .5f;
	}

	if (cKeyboardController->IsKeyPressed(GLFW_KEY_G))
	{
		//cInventoryM->AddItem("Shuriken", ITEM_SHURIKEN);
	}
}

void CInventory::Init()
{
	//m_Items.insert(pair<int, CItem>(0,CItem(2)));

	SetItem(0, "Shuriken",ITEM_SHURIKEN, 0, 100);
	SetItem(1, "Potion", ITEM_POTION, 0, 100, 2);
	SetItem(2, "Hook", ITEM_HOOK, 0, 100);
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
}

int CInventory::GetNumofUniqueItems()
{
	return m_Items.size();
}

void CInventory::UseItem(void)
{
	if (m_Items.find(iCurrentIndex) != m_Items.end()) //if item is existed
	{
		m_Items.at(iCurrentIndex).Use();
	}
	else
		cout << "Item does not exist\n";
}
