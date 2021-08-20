#include "InventoryM.h"

CInventoryM::CInventoryM()
	:iItemIndex(1)
{
}

CInventoryM::~CInventoryM()
{
	//clean up
	m_Items.clear();
	m_Inventory.clear();
	m_potion.clear();
}

void CInventoryM::Init()
{
	AddItem("Shuriken", ITEM_SHURIKEN);
	vec2Size = glm::vec2(25, 25);
	AddItem("Potion", ITEM_POTION);
	vec2Size = glm::vec2(25, 25);
	AddItem("Hook", ITEM_HOOK);
	vec2Size = glm::vec2(25, 25);

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
	if(iID == ITEM_POTION)
		m_potion.push_back(iID);
	if (iID == ITEM_SHURIKEN)
	{
		for (int i = 0; i < 100; i++)
		{
			m_shuriken.push_back(i);
		}
	}

	cout << "Added item " << sItemName << " and the size of map is " << m_Items.size()<<'\n';
	cout << "Vector size is \n" << m_potion.size();
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
		if (m_potion.size() <= 0)
			return;
		cout << "you are using potion\n";
		m_potion.erase(m_potion.begin()); //erase 1 potion from the vector
		cout << "Vector size is " << m_potion.size()<<'\n';
	}
	else if (iItemIndex == 2)
	{
		cout << "you are using hook\n";
		//use hook
	}
}
