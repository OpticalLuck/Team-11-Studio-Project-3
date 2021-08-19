#include "InventoryMain.h"

CInventoryMain::CInventoryMain(void)
	//: iItemIndex(2) 
	: cInventoryItem(NULL)
	, cInventoryManager(NULL)

{
}

CInventoryMain::~CInventoryMain(void)
{
}

bool CInventoryMain::Init(void)
{
	cInventoryManager = CInventoryManager::GetInstance();
	cInventoryItem = new CInventoryItem;
	//cInventoryItem = nullptr;
	return true;
}

void CInventoryMain::AddItem(string item)
{
	if (item == "Shuriken")
	{
		cInventoryItem = cInventoryManager->Add("Shuriken", 2, 999, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);
	}
	else if (item == "Potion")
	{
		cInventoryItem = cInventoryManager->Add("Potion", 3, 2, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);
	}
	else if (item == "Hook")
	{
		cInventoryItem = cInventoryManager->Add("Hook", 4, 2, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);
	}
	else
		return;


}

void CInventoryMain::GetItem(string item)
{
	if (item == "Shuriken")
	{
		cInventoryItem = cInventoryManager->GetItem("Shuriken");
	}
	else if (item == "Potion")
	{
		cInventoryItem = cInventoryManager->GetItem("Potion");
	}
	else if (item == "Hook")
	{
		cInventoryItem = cInventoryManager->GetItem("Hook");
	}
	else
		return;
}

//void CInventoryMain::NavigateIndex(string direction)
//{
//	if (direction == "DOWN")
//	{
//		++iItemIndex;
//		//if (iItemIndex > 2)
//			//iItemIndex = 1;
//	}
//	if (direction == "UP")
//	{
//		--iItemIndex;
//		//if (iItemIndex <= 0)
//			//iItemIndex = 2;
//	}
//
//}
//int CInventoryMain::GetItemIndex(void)
//{
//	return iItemIndex;
//}

unsigned int CInventoryMain::GetTexture(void) const
{
	return cInventoryItem->GetTextureID();
}

int CInventoryMain::GetItemCount(void)
{
	return cInventoryItem->GetCount();
}

glm::vec2 CInventoryMain::GetItemSize(void)
{
	return cInventoryItem->vec2Size;
}
