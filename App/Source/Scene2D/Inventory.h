#pragma once
#include "Item.h"
#include "../InputHandler/CInputHandler.h"

#include <map>
#include <vector>

class CInventory
{
public:
	CInventory(std::string sName, CPlayer2D* target);
	~CInventory();

	void AddItem(std::string itemName, int iCount = 1);

	void Update(double dElapsedTime, 
				int iTempFrameCounter,
				std::vector<std::array<KeyInput, KEYBOARD_INPUTS::KEY_TOTAL>> m_KeyboardInputs, 
				std::vector<std::array<MouseInput, MOUSE_INPUTS::MOUSE_TOTAL>> m_MouseInputs);

	void Init();

	void SetItem(int iIndex, std::string sName, int iID, int iMinVal, int iMaxVal, int iSetVal = 0);

	CItem& GetItem(int iIndex);

	int GetNumofUniqueItems();

	void UseItem(void);

	void SetPlayer(CPlayer2D* player);
	

	void BackupInventory() { m_SavedItems = m_Items; iSavedIndex = iCurrentIndex; }
	void RestoreBackup() { m_Items = m_SavedItems; iCurrentIndex = iSavedIndex; }

	void CloneInventoryData(CPlayer2D* target);

	std::map<int, CItem> GetAllItems() const { return m_Items; }

	float fCooldown;

	int iCurrentIndex;
	int iSavedIndex;

	std::string sName;
private:
	std::map<int, CItem>m_Items;
	std::map<int, CItem>m_SavedItems;
	CPlayer2D* linkedPlayer;

	static CItem itemDef;
};