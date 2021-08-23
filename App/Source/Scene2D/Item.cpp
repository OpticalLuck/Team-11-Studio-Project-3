#include "Item.h"
#include "Object2D.h"

CItem::CItem(int ID)
	: iID(ID)
{
}

CItem::CItem(int ID, std::string _name)
	: iID(ID)
	, sName(_name)
{
}

CItem::CItem()
{
}

CItem::~CItem()
{
}

int CItem::get_ID()
{
	return iID;
}

int CItem::set_ID(int _ID)
{
	iID = _ID;
	return iID;
}

std::string CItem::GetName()
{
	return sName;
}

void CItem::set_Name(std::string _name)
{
	sName = _name;
}

void CItem::Use(void)
{
	if (iCount > iMinCount)
	{
		iCount--;
		switch (iID)
		{
		case OBJECT_TYPE::CONSUMABLES_POTION:
			cout << "You are using potion\n";
			cout << "iCount is " << iCount << '\n';
			break;
		case OBJECT_TYPE::EQUIPMENTS_HOOK:
			cout << "You are using Item hook\n";
			cout << "iCount is " << iCount << '\n';
			break;
		case OBJECT_TYPE::PROJECTILES_SHURIKEN:
			cout << "You are using Item shuriken\n";
			cout << "iCount is " << iCount << '\n';
			break;
		}
	}
}



