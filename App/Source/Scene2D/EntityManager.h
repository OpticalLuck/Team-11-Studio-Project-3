#pragma once 
//#include "Primitives/Entity2D.h"
#include "Player2D.h"
#include "Enemy2D.h"
#include <vector>
#include <iostream>
#include "Map2D.h"

class CEntityManager : public CEntity2D , public CSingletonTemplate<CEntityManager>
{
public:
	CEntityManager();
	~CEntityManager();

	void Update();
	void PlayerHandler();
	void PlayerInit();

	void InitCloneTemplate(void);

	void EnemyHandler();
	void EnemyInit();

	bool CheckCollision(CEntity2D* type1, CEntity2D* type2);

	void RemoveEntity(string type, int amount);

protected:
	CPlayer2D* cCloneTemplate;

	CPlayer2D* cPlayer2D;
	CEnemy2D* cEnemy2D;
	CMap2D* cMap2D;
	std::vector<CEntity2D*> m_playerList;
	std::vector<CEntity2D*> m_enemyList;
	std::vector<CEntity2D*> m_cloneList;
};