#pragma once
#include "DesignPatterns\SingletonTemplate.h"
#include "Inventory.h"
#include <string>

class CInventoryManager : public CSingletonTemplate<CInventoryManager>
{
	friend CSingletonTemplate<CInventoryManager>;
public:
	CInventoryManager();
	~CInventoryManager();

	void Add(std::string sName, CPlayer2D* target);

	void Use(std::string sName);

	CInventory* Get(std::string sName);
	void DeleteInventory(std::string sName);


	//navigate the item
	void NavigateIndex(std::string direction);

	
	void UseItem(void);

private:
	std::map<std::string, CInventory>m_Inventory; //store a list of inventory for clone/player
	CInventory* cActiveInventory;
};