#pragma once

//Include Entities
#include "Player2D.h"
#include "Enemy2D.h"
#include "Interactables.h"
#include "Obstacle2D.h"

#include <vector>
#include <iostream>
#include "Map2D.h"
//Include keyboard controller
#include "Inputs/KeyboardController.h"
#include "InventoryManager.h"

//Factories
#include "../App/Source/Factory/EnemyFactory.h"

class CBoss2D;
class Projectiles;

//System debug
#include "System/Debug.h"

class CEntityManager : public CSingletonTemplate<CEntityManager>
{
	friend class CSingletonTemplate<CEntityManager>;
public:
	//Inits
	bool EntityManagerInit(void);

	//Renders
	void RenderEnemy(void);
	void RenderClone(void);
	void RenderPlayer(void);
	void RenderBullets(void);
	void RenderInteractables(void);
	void RenderObstacles(void);

	void Update(const double dElapsedTime);
	CPlayer2D* Clone(void);
	bool CheckCollision(CEntity2D* type1, CEntity2D* type2);
	void RemoveEntity(string type, int amount);

	CPlayer2D* GetPlayer();
	CBoss2D* GetBoss();
	int GetCurrRound(void);

	void PushEnemy(CEnemy2D*);
	void PushBullet(Projectiles*);
	void PushInteractables(Interactables*);
	void PushObstacles(Obstacle2D*);

	std::vector<CPlayer2D*> GetAllPlayers(void);
	std::vector<CEnemy2D*> GetAllEnemies(void);
	std::vector<CEntity2D*> GetAllLivingEntities(void);
	std::vector<Interactables*> GetAllInteractables(void);
	std::vector<Obstacle2D*> GetallObstacles(void);
	std::vector<CEntity2D*> GetAllSolidEntities(void);

	void Clear(void);

protected:
	CEntityManager();
	~CEntityManager();

	CInputHandler* cInputHandler;
	CKeyboardController* cKeyboardController;

	CPlayer2D* cPlayer2D;

	CBoss2D* cBoss2D;
	CEnemy2D* cEnemy2D;

	CMap2D* cMap2D;

	CInventoryManager* CInventoryManager;

	//list of vectors
	std::vector<Interactables*> m_interactableList;
	std::vector<CEnemy2D*> m_enemyList;
	std::vector<Projectiles*> m_BulletList;
	std::vector<CPlayer2D*> m_cloneList;
	std::vector<Obstacle2D*> m_ObstacleList;

	int currRound; //Current round in game. First is 0 and last is 4 (Will be storing enemies prev attacks etc using this)

	//Factories
	EnemyFactory enemyFactory;
};
