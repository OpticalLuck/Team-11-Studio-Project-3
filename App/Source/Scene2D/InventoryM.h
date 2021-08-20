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
	
	//to initialize the item into inventory manager
	void Init(); 

	//brief: returns the requested name
	CItem GetItem(std::string name);

	//navigate the item
	void NavigateIndex(std::string direction);
	//returns the item index
	int GetItemIndex(void);
	//remove item (USE SMALL LETTER)
	void RemoveItem(std::string name);
	//use the item when requested
	void UseItem();

	void AddItem(std::string sItemName, int iID); //add item into the map of items

private:
	std::map<std::string, CItem>m_Items; //stores the list of items into the map
	std::map<std::string, CInventory>m_Inventory; //store a list of inventory for clone/player
	int iItemIndex;


};