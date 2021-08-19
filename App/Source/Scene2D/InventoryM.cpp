#include "InventoryM.h"
#include "Object2D.h"

CInventoryM::CInventoryM()
{
}

CInventoryM::~CInventoryM()
{
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

void CInventoryM::AddItem(std::string sItemName, int iID)
{
	m_Items.insert(std::make_pair(sItemName, CItem(iID, sItemName)));
}
