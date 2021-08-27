#pragma once

//Include Entities
#include "Player2D.h"
#include "Enemy2D.h"
#include "Interactables.h"
#include "Obstacle2D.h"

#include <vector>
#include <iostream>
#include <queue>
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
	bool EntityManagerInit(const unsigned int totalLevels);

	//Renders
	void RenderEnemy(void);
	void RenderClone(void);
	void RenderPlayer(void);
	void RenderBullets(void);
	void RenderInteractables(void);
	void RenderObstacles(void);

	void Update(const double dElapsedTime);
	CPlayer2D* Clone(void);
	int GetNoOfAvailableCloneID();
	bool CheckCollision(CEntity2D* type1, CEntity2D* type2);
	void RemoveEntity(string type, int amount);

	bool InitPlayer();
	CPlayer2D* GetPlayer();
	CBoss2D* GetBoss();
	int GetCurrRound(void);

	void PushEnemy(CEnemy2D*, const unsigned int currLevel);
	void PushBullet(Projectiles*, const unsigned int currLevel);
	void PushInteractables(Interactables*, const unsigned int currLevel);
	void PushObstacles(Obstacle2D*, const unsigned int currLevel);

	std::vector<CPlayer2D*> GetAllPlayers(void);
	std::vector<CEnemy2D*> GetAllEnemies(void);
	std::vector<CEntity2D*> GetAllLivingEntities(void);
	std::vector<Interactables*> GetAllInteractables(void);
	std::vector<Obstacle2D*> GetallObstacles(void);
	std::vector<CEntity2D*> GetAllSolidEntities(void);
	std::vector<Projectiles*> GetAllBullets(void);

	void Clear(void);

	//For recording purposes
	void SavePrevious(void);
	void LoadPrevious(void);

protected:
	CEntityManager();
	~CEntityManager();

	CInputHandler* cInputHandler;
	CKeyboardController* cKeyboardController;

	CPlayer2D* cPlayer2D;
	std::queue<int> qclone_ID;

	std::vector<CBoss2D*> cBoss2D;
	CEnemy2D* cEnemy2D;

	CMap2D* cMap2D;

	CInventoryManager* CInventoryManager;

	//list of vectors
	std::vector<std::vector<Interactables*>> m_interactableList;
	std::vector<std::vector<CEnemy2D*>> m_enemyList;
	std::vector<std::vector<Projectiles*>> m_BulletList;
	std::vector<CPlayer2D*> m_cloneList;
	std::vector<std::vector<Obstacle2D*>> m_ObstacleList;

	//Recorded value
	std::vector<Projectiles*> m_PrevBulletList;

	int currRound; //Current round in game. First is 0 and last is 4 (Will be storing enemies prev attacks etc using this)

	//Factories
	// EnemyFactory enemyFactory;
};
