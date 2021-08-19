#pragma once
#include "Item.h"
#include "DesignPatterns\SingletonTemplate.h"

class CInventoryM : public CSingletonTemplate<CInventoryM>
{
	friend CSingletonTemplate<CInventoryM>;
public:
	CInventoryM();
	~CInventoryM();

	void Init();

	


private:


};