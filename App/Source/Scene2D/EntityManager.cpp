#include "EntityManager.h"


CEntityManager::CEntityManager()
	: cPlayer2D(NULL)
	, cEnemy2D(NULL)
	, cMap2D(NULL)
{
}

CEntityManager::~CEntityManager()
{
}

void CEntityManager::PlayerInit()
{
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(1, true, uiRow, uiCol) == true)
	{
		cPlayer2D = new CPlayer2D;
		cPlayer2D->collidable = true;
		m_playerList.push_back(cPlayer2D);
	}
		
}

void CEntityManager::InitCloneTemplate(void)
{
	cCloneTemplate = new CPlayer2D();
	cCloneTemplate->SetClone(true);
	// initialise all default values
	// find position to spawn in map
}

void CEntityManager::EnemyInit()
{
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(300, true, uiRow, uiCol) == true)
	{
		cEnemy2D = new CEnemy2D;
		cEnemy2D->collidable = true;
		m_enemyList.push_back(cEnemy2D);
	}
}

bool CEntityManager::CheckCollision(CEntity2D* type1, CEntity2D* type2)
{
	if (type1->type == CEntity2D::PLAYER && type2->type == CEntity2D::ENEMY)
	{
		return false;
	}
	else
		return false;

}

void CEntityManager::RemoveEntity(string type, int amount)
{
	if (type == "PLAYER")
	{
		m_playerList.erase(m_playerList.begin());
	}
	else if (type == "ENEMY")
	{
		for (int i = 0; i < amount; i++)
		{
			m_enemyList.erase(m_enemyList.begin());
		}
	}
}

void CEntityManager::Update()
{
	for (std::vector<CEntity2D*>::iterator it = m_playerList.begin(); it != m_playerList.end(); ++it)
	{
		CEntity2D* player = (CEntity2D*)*it;
		{
			for (std::vector<CEntity2D*>::iterator it2 = m_enemyList.begin(); it2 != m_enemyList.end(); ++it2)
			{
				CEntity2D* enemy = (CEntity2D*)*it2;
				if (CheckCollision(player, enemy))
				{
					//reduce health or some shit
				}
			}
		}
	}
}

void CEntityManager::PlayerHandler()
{

}


void CEntityManager::EnemyHandler()
{
}


