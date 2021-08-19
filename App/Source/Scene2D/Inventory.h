#pragma once
#include "Item.h"

#include <map>
#include <vector>

class CInventory
{
public:
    enum ITEM_TYPE
    {
        ITEM_NONE = 0,
        ITEM_HOOK,
        ITEM_POTION,
        ITEM_TOTAL,
    };
    ITEM_TYPE item;
    CInventory(ITEM_TYPE item = ITEM_NONE);



	CInventory();
	~CInventory();


	


private:
    std::map<int ,CItem*> ItemCount;

};