#pragma once
#include "Item.h"
#include "DesignPatterns\SingletonTemplate.h"
#include "Inventory.h"

#include <string>

class CInventoryM : public CSingletonTemplate<CInventoryM>
{
	friend CSingletonTemplate<CInventoryM>;
public:
	CInventoryM();
	~CInventoryM();

	void Init();

	CItem GetItem(std::string name);

	CItem GetTextureID(int ID);

	void AddItem(std::string sItemName, int iID);


private:
	std::vector<CItem*>m_Items;
	std::vector<CInventory*> m_Inventory;

	CItem Item;

};