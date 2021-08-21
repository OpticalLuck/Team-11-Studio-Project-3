#pragma once
#include "Item.h"

#include <map>
#include <vector>

class CInventory
{
public:
	CInventory(std::string sName);
	~CInventory();

	void AddItem(int iIndex, int iID, int iCount = 1);

	void Update(double dElapsedTime);

	void Init();

	void SetItem(int iIndex, std::string sName, int iID, int iMinVal, int iMaxVal, int iSetVal = 0);

	CItem& GetItem(int iIndex);

	int GetNumofUniqueItems();

	void UseItem(void);
	
	float fCooldown;

	int iCurrentIndex;

	std::string sName;
private:
	std::map<int, CItem>m_Items;

};