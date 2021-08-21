#pragma once 

#include "Player2D.h"
#include "Enemy2D.h"
#include <vector>
#include <iostream>
#include "Map2D.h"
//Include keyboard controller
#include "Inputs/KeyboardController.h"
#include "InventoryManager.h"

class CBoss2D;

class CEntityManager : public CEntity2D , public CSingletonTemplate<CEntityManager>
{
public:
	CEntityManager();
	~CEntityManager();

	//Inits
	bool EntityManagerInit(void);
	
	//Renders
	void RenderEnemy(void);
	void RenderClone(void);
	void RenderPlayer(void);

	void Update(const double dElapsedTime);
	bool Clone(void);
	bool CheckCollision(CEntity2D* type1, CEntity2D* type2);
	void RemoveEntity(string type, int amount);

	CPlayer2D* GetPlayer();
	int GetCurrRound(void);

	std::vector<CPlayer2D*> GetAllPlayers(void);

protected:
	CKeyboardInputHandler* cKeyboardInputHandler;
	CKeyboardController* cKeyboardController;

	CPlayer2D* cCloneTemplate;
	CPlayer2D* cPlayer2D;

	CBoss2D* cBoss2D;
	CEnemy2D* cEnemy2D;

	CMap2D* cMap2D;

	CInventoryManager* CInventoryManager;

	//list of vectors
	std::vector<CEnemy2D*> m_enemyList;
	std::vector<CEntity2D*> m_cloneList;

	int currRound; //Current round in game. First is 0 and last is 4 (Will be storing enemies prev attacks etc using this)
};