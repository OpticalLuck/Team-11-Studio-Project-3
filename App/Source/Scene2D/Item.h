#pragma once
#include <vector>
#include "iostream"
#include "..\TextureManager\TextureManager.h"

class CItem
{
public:
	CItem(int ID);
	~CItem();

private:
	unsigned int ID;
	CTextureManager* cTextureManager;

};