#pragma once
#include <vector>
#include <iostream>
#include <string>

class CPlayer2D;

class CItem
{
public:
	CItem(int ID);
	CItem(int ID, std::string _name);
	CItem();
	~CItem();
	int get_ID();
	int set_ID(int _ID);;
	std::string GetName();
	void set_Name(std::string _name);
	void Use(CPlayer2D* user);

	int iCount; 
	int iMinCount;
	int iMaxCount;
private:
	unsigned int iID;
	std::string sName;
};