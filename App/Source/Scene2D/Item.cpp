#include "Item.h"

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

void CItem::set_Name(std::string _name)
{
	sName = _name;
}



