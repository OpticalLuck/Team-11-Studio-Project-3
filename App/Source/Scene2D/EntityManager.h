#pragma once 
#include "Primitives/Entity2D.h"
#include <vector>
#include <iostream>
#include "Map2D.h"

class EntityManager : public CEntity2D
{
public:
	EntityManager();
	~EntityManager();

	void Update();
	void playerHandler();
	void playerInit();

	void enemyHandler();
	void enemyInit();

	bool checkCollision(CEntity2D* type1, CEntity2D* type2);

	void removeEntity(string type, int amount);

protected:
	CEntity2D* player;
	CEntity2D* enemy;
	CMap2D* cMap2D;
	std::vector<CEntity2D*> playerList;
	std::vector<CEntity2D*> enemyList;

};