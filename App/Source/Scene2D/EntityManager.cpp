#include "EntityManager.h"


EntityManager::EntityManager()
{
}

EntityManager::~EntityManager()
{
}

void EntityManager::playerInit()
{
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(1, true, uiRow, uiCol) == true)
	{
		player = new CEntity2D(CEntity2D::PLAYER);
		player->collidable = true;
		playerList.push_back(new CEntity2D(CEntity2D::PLAYER));
	}
		
}

void EntityManager::enemyInit()
{
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(300, true, uiRow, uiCol) == true)
	{
		enemy = new CEntity2D(CEntity2D::ENEMY);
		enemy->collidable = true;
		enemyList.push_back(new CEntity2D(CEntity2D::ENEMY));
	}
}

bool EntityManager::checkCollision(CEntity2D* type1, CEntity2D* type2)
{
	if (type1->type == CEntity2D::PLAYER && type2->type == CEntity2D::ENEMY)
	{
		return true;
	}
	else
		return false;

}

void EntityManager::removeEntity(string type, int amount)
{
	if (type == "PLAYER")
	{
		playerList.erase(playerList.begin());
	}
	else if (type == "ENEMY")
	{
		for (int i = 0; i < amount; i++)
		{
			enemyList.erase(enemyList.begin());
		}
	}
}

void EntityManager::Update()
{
	for (std::vector<CEntity2D*>::iterator it = playerList.begin(); it != playerList.end(); ++it)
	{
		CEntity2D* player = (CEntity2D*)*it;
		{
			for (std::vector<CEntity2D*>::iterator it2 = enemyList.begin(); it2 != enemyList.end(); ++it2)
			{
				CEntity2D* enemy = (CEntity2D*)*it2;
				if (checkCollision(player, enemy))
				{
					//reduce health or some shit
				}
			}
		}
	}
}

void EntityManager::playerHandler()
{

}


void EntityManager::enemyHandler()
{
}


