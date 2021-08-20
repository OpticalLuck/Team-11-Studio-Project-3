#include "InventoryM.h"
#include "Object2D.h"

CInventoryM::CInventoryM()
	:iItemIndex(1)
{
}

CInventoryM::~CInventoryM()
{
	//clean up
	m_Items.clear();
	m_Inventory.clear();
}

void CInventoryM::Init()
{
	AddItem("Shuriken", ITEM_SHURIKEN);
	AddItem("Potion", ITEM_POTION);
	AddItem("Hook", ITEM_HOOK);

}

CItem CInventoryM::GetItem(std::string name)
{
	return m_Items.at(name);
}

void CInventoryM::NavigateIndex(std::string direction)
{
	if (direction == "DOWN")
	{
		++iItemIndex;
		if (iItemIndex > 2)
			iItemIndex = 2;
	}
	if (direction == "UP")
	{
		--iItemIndex;
		if (iItemIndex <= 0)
			iItemIndex = 1;
	}
}

void CInventoryM::AddItem(std::string sItemName, int iID)
{
	m_Items.insert(std::make_pair(sItemName, CItem(iID, sItemName)));
}

int CInventoryM::GetItemIndex(void)
{
	return iItemIndex;
}

void CInventoryM::RemoveItem(std::string name)
{
	if (name == "SHURIKEN")
		m_Items.erase("SHURIKEN");
	if (name == "potion")
		m_Items.erase("POTION");
	if (name == "hook")
		m_Items.erase("HOOK");
}

void CInventoryM::UseItem()
{
	if (iItemIndex == 1)
	{
		cout << "you are using potion\n";
		//use potion
	}
	else if (iItemIndex == 2)
	{
		cout << "you are using hook\n";
		//use hook
	}
}
