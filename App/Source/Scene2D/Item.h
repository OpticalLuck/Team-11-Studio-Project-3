#pragma once
#include <vector>
#include "iostream"
#include <string>
#include "..\TextureManager\TextureManager.h"

class CItem
{
public:
	CItem(int ID);
	CItem(int ID, std::string _name);
	CItem();
	~CItem();
	int get_ID();
	int set_ID(int _ID);
	void set_Name(std::string _name);

private:
	unsigned int iID;
	std::string sName;


};