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
	
	/*brief: to initialize the item into inventory manager*/
	void Init(); 

	/*brief: returns the requested name*/
	CItem GetItem(std::string name);

	void NavigateIndex(std::string direction);
	int GetItemIndex(void);


private:
	std::map<std::string, CItem>m_Items; //stores the list of items into the map
	std::map<std::string, CInventory>m_Inventory; //store a list of inventory for clone/player
	int iItemIndex;

protected:
	void AddItem(std::string sItemName, int iID);
};