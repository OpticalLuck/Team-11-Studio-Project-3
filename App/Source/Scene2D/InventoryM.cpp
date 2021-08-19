#include "InventoryM.h"

void CInventoryM::Init()
{


}

CItem CInventoryM::GetItem(std::string name)
{
	return CItem();
}

CItem CInventoryM::GetTextureID(int ID)
{
	if (ID == 2)
	{
		GetItem("Shuriken");
	}
}

void CInventoryM::AddItem(std::string sItemName, int iID)
{
	if (sItemName == "Shuriken")
	{
		CItem* item = new CItem(2);
		m_Items.push_back(item);
	}
}
