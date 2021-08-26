#include "EntityManager.h"

#include "MobEnemy2D.h"

#include "Boss2D.h"

#include "Bullet2D.h"
#include "Projectiles.h"

CEntityManager::CEntityManager()
	: cPlayer2D(NULL)
	, cEnemy2D(NULL)
	, cMap2D(NULL)
	, cBoss2D(NULL)
	, cKeyboardController(NULL)
	, cInputHandler(NULL)
	, CInventoryManager(NULL)
	, currRound(0)
{
}

CEntityManager::~CEntityManager()
{
	Clear();
}

bool CEntityManager::EntityManagerInit(void)
{
	// Create and initialise the Map 2D
	cMap2D = CMap2D::GetInstance();
	// Set a shader to this class
	cMap2D->SetShader("2DShader");
	// Store the keyboard controller singleton instance here
	cKeyboardController = CKeyboardController::GetInstance();
	cInputHandler = CInputHandler::GetInstance();

	unsigned int uiRow = -1;
	unsigned int uiCol = -1;

	//player init
	if (cMap2D->FindValue(1, uiRow, uiCol) == true)
	{
		cPlayer2D = new CPlayer2D;
		// Pass shader to cPlayer2D
		cPlayer2D->SetShader("2DColorShader");
		// m_playerList.push_back(cPlayer2D);
		// Initialise the instance
		if (cPlayer2D->Init() == false)
		{
			cout << "Failed to load CPlayer2D" << endl;
			return false;
		}
	}

	//enemy init
	while (cMap2D->FindValue(300, uiRow, uiCol)) {
		m_enemyList.push_back(enemyFactory.CreateEnemy(300));
	}

	while (cMap2D->FindValue(301, uiRow, uiCol)) {
		m_enemyList.push_back(enemyFactory.CreateEnemy(301));
	}

	while (cMap2D->FindValue(302, uiRow, uiCol)) {
		m_enemyList.push_back(enemyFactory.CreateEnemy(302));
	}

	while (cMap2D->FindValue(303, uiRow, uiCol)) {
		m_enemyList.push_back(enemyFactory.CreateEnemy(303));
	}

	while (cMap2D->FindValue(304, uiRow, uiCol)) {
		m_enemyList.push_back(enemyFactory.CreateEnemy(304));
	}

	//Boss initialisation
	if (cMap2D->FindValue(305, uiRow, uiCol)) {
		cBoss2D = dynamic_cast<CBoss2D*>(enemyFactory.CreateEnemy(305));

		if (cMap2D->FindValue(305, uiRow, uiCol)) {
			DEBUG_MSG("ERROR: TOO MANY BOSS IN LEVEL. THERE SHOULD ONLY BE ONE BOSS");
			return false;
		}
		if (cBoss2D->Init() == false)
		{
			cout << "Failed to load CBoss2D" << endl;
			return false;
		}
	}

	for (Interactables* i : m_interactableList)
	{
		i->Init();
	}

	//clone init
	//cCloneTemplate = new CPlayer2D();
	//if (cCloneTemplate->Init(cPlayer2D->GetCheckpoint(),m_cloneList.size()) == false)
	//{
	//	cout << "Failed to load clone" << endl;
	//	return false;
	//}
	//// initialise all default values
	//cCloneTemplate->SetClone(true);
	//cCloneTemplate->SetShader("2DColorShader");
	//// find position to spawn in map
	//if (cMap2D->FindValue(1, uiRow, uiCol) == true)
	//{
	//	cCloneTemplate->vTransform = glm::vec2(uiCol, uiRow);
	//}
	currRound = 0;

	return true;
}

std::vector<CEnemy2D*> CEntityManager::GetAllEnemies(void) {
	std::vector<CEnemy2D*> arr;
	arr.push_back(cBoss2D);
	arr.insert(arr.end(), m_enemyList.begin(), m_enemyList.end());

	arr.erase(std::remove(arr.begin(), arr.end(), nullptr), arr.end());

	return arr;
}

std::vector<CEntity2D*> CEntityManager::GetAllLivingEntities(void) {
	std::vector<CEnemy2D*> enemyArr = GetAllEnemies();
	std::vector<CEntity2D*> convertedEnemyArr(enemyArr.begin(), enemyArr.end());

	std::vector<CPlayer2D*> playerArr = GetAllPlayers();
	std::vector<CEntity2D*> convertedPlayerArr(playerArr.begin(), playerArr.end());

	std::vector<CEntity2D*> totalArr;
	totalArr.reserve(convertedPlayerArr.size() + convertedEnemyArr.size());
	totalArr.insert(totalArr.end(), convertedPlayerArr.begin(), convertedPlayerArr.end());
	totalArr.insert(totalArr.end(), convertedEnemyArr.begin(), convertedEnemyArr.end());

	return totalArr;
}

std::vector<Interactables*> CEntityManager::GetAllInteractables(void)
{
	return m_interactableList;
}

std::vector<Obstacle2D*> CEntityManager::GetallObstacles(void)
{
	return std::vector<Obstacle2D*>(m_ObstacleList);
}

void CEntityManager::PushEnemy(CEnemy2D* enemy) {
	if (enemy)
		m_enemyList.push_back(enemy);
	else
		DEBUG_MSG("ENEMY NOT ADDED AS IT IS A NULLPTR.");
}

CPlayer2D* CEntityManager::Clone(void)
{
	CPlayer2D* clone = new CPlayer2D();
	clone->SetShader("2DColorShader");

	if (!clone->Init(cPlayer2D->GetCheckpoint(), m_cloneList.size()))
	{
		DEBUG_MSG("Failed to clone Player");
		return nullptr;
	}
	clone->SetClone(true);
	clone->SetKeyInputs(cInputHandler->GetAllKeyboardInputs());
	clone->SetMouseInputs(cInputHandler->GetAllMouseInputs());
	m_cloneList.push_back(clone);

	return clone;
}


bool CEntityManager::CheckCollision(CEntity2D* type1, CEntity2D* type2)
{
	if (type1->type == CEntity2D::ENTITY_TYPE::PLAYER && type2->type == CEntity2D::ENTITY_TYPE::ENEMY)
	{
		return false;
	}
	else
		return false;

}

void CEntityManager::RemoveEntity(string type, int amount)
{
	if (type == "ENEMY")
	{
		for (int i = 0; i < amount; i++)
		{
			m_enemyList.erase(m_enemyList.begin());
			return;
		}
	}
	else if (type == "CLONE")
	{
		for (int i = 0; i < amount; i++)
		{
			m_cloneList.erase(m_cloneList.begin());
			return;
		}
	}
	else
		return;
}

CPlayer2D* CEntityManager::GetPlayer()
{
	return cPlayer2D;
}

CBoss2D* CEntityManager::GetBoss()
{
	return cBoss2D;
}

std::vector<CPlayer2D*> CEntityManager::GetAllPlayers(void) 
{
	std::vector<CPlayer2D*> arr;
	
	arr.push_back(cPlayer2D); //Add controllable player

	for (unsigned i = 0; i < m_cloneList.size(); i++) 
	{ //Add the other clones to the vector
		arr.push_back(static_cast<CPlayer2D*>(m_cloneList[i])); 
	}

	return arr;
}

int CEntityManager::GetCurrRound(void) 
{
	return currRound;
}

void CEntityManager::RenderEnemy(void)
{
	for (unsigned i = 0; i < m_enemyList.size(); i++)
	{
		m_enemyList[i]->PreRender();
		m_enemyList[i]->Render();
		m_enemyList[i]->PostRender();

		m_enemyList[i]->RenderCollider();
	}

	if (cBoss2D) {
		cBoss2D->PreRender();
		cBoss2D->Render();
		cBoss2D->PostRender();
		
		cBoss2D->RenderCollider();
	}
}

void CEntityManager::RenderBullets(void) {
	for (unsigned i = 0; i < m_BulletList.size(); i++) {
		m_BulletList[i]->PreRender();
		m_BulletList[i]->Render();
		m_BulletList[i]->PostRender();
		
		m_BulletList[i]->RenderCollider();
	}
}

void CEntityManager::RenderInteractables(void)
{
	for (Interactables* i : m_interactableList)
	{
		i->PreRender();
		i->Render();
		i->PostRender();

		i->RenderCollider();
	}
}

void CEntityManager::RenderObstacles(void)
{
	for (Obstacle2D* i : m_ObstacleList)
	{
		i->PreRender();
		i->Render();
		i->PostRender();

		i->RenderCollider();
	}
}

void CEntityManager::RenderClone(void)
{
	for (unsigned i = 0; i < m_cloneList.size(); ++i)
	{
		m_cloneList[i]->PreRender();
		m_cloneList[i]->Render();
		m_cloneList[i]->PostRender();

		m_cloneList[i]->RenderCollider();
	}
}

void CEntityManager::RenderPlayer(void)
{
	// Call the CPlayer2D's PreRender()
	cPlayer2D->PreRender();
	// Call the CPlayer2D's Render()
	cPlayer2D->Render();
	// Call the CPlayer2D's PostRender()
	cPlayer2D->PostRender();

	cPlayer2D->RenderCollider();
}

void CEntityManager::Update(const double dElapsedTime)
{
	
	for (Interactables* i : m_interactableList)
	{
		i->Update(dElapsedTime);
	}

	for (unsigned i = 0; i < m_cloneList.size(); ++i)
	{
		if (m_cloneList[i]->iTempFrameCounter - m_cloneList[i]->iFrameCounterEnd > 180)
		{
			delete m_cloneList[i];
			m_cloneList[i] = nullptr;
			m_cloneList.erase(std::remove(std::begin(m_cloneList), std::end(m_cloneList), nullptr), std::end(m_cloneList));
			continue;
		}

		m_cloneList[i]->Update(dElapsedTime);
	}

	// Call the cPlayer2D's update method before Map2D as we want to capture the inputs before map2D update
	cPlayer2D->Update(dElapsedTime);

	if (cBoss2D)
		cBoss2D->Update(dElapsedTime);


	// Call all the cEnemy2D's update method before Map2D 
	// as we want to capture the updates before map2D update
	for (unsigned i = 0; i < m_enemyList.size(); i++) {
		m_enemyList[i]->Update(dElapsedTime);

		//Delete conditions
		if (m_enemyList[i]->GetHealth() <= 0) {
			delete m_enemyList[i];
			m_enemyList[i] = nullptr;
		}
	}

	m_enemyList.erase(std::remove(m_enemyList.begin(), m_enemyList.end(), nullptr), m_enemyList.end()); //Remove any nullptrs in the array

	//Call enemy bullets
	for (unsigned i = 0; i < m_BulletList.size(); i++) {
		m_BulletList[i]->Update(dElapsedTime);

		if (dynamic_cast<Projectiles*>(m_BulletList[i]))
		{
			if (dynamic_cast<Projectiles*>(m_BulletList[i])->bDestroyed || dynamic_cast<Projectiles*>(m_BulletList[i])->bOutsideBoundary())
			{
				DEBUG_MSG("bullet deleted");
				delete m_BulletList[i];
				m_BulletList[i] = nullptr;
			}
		}
	}

	//Remove any nullptrs in bullet array
	m_BulletList.erase(std::remove(m_BulletList.begin(), m_BulletList.end(), nullptr), m_BulletList.end());

	for (Obstacle2D* i : m_ObstacleList)
	{
		i->Update(dElapsedTime);
	}

	//Keyboard inputs
	if (cKeyboardController->IsKeyPressed(GLFW_KEY_C))
	{
		Clone();
	}
}

void CEntityManager::PushBullet(Projectiles* bullet) {
	m_BulletList.push_back(bullet);
}

void CEntityManager::PushInteractables(Interactables* interactable)
{
	m_interactableList.push_back(interactable);
}

void CEntityManager::Clear(void)
{
	if (cPlayer2D) {
		delete cPlayer2D;
		cPlayer2D = nullptr;
	}

	if (cBoss2D) {
		delete cBoss2D;
		cBoss2D = nullptr;
	}

	for (unsigned i = 0; i < m_enemyList.size(); i++) {
		delete m_enemyList[i];
		m_enemyList[i] = nullptr;
	}
	m_enemyList.clear();

	for (unsigned i = 0; i < m_cloneList.size(); i++) {
		delete m_cloneList[i];
		m_cloneList[i] = nullptr;
	}
	m_cloneList.clear();

	for (unsigned i = 0; i < m_BulletList.size(); i++) {
		delete m_BulletList[i];
		m_BulletList[i] = nullptr;
	}
	m_BulletList.clear();

	for (unsigned i = 0; i < m_interactableList.size(); i++) {
		delete m_interactableList[i];
		m_interactableList[i] = nullptr;
	}
	m_interactableList.clear();

	for (unsigned i = 0; i < m_ObstacleList.size(); i++) {
		delete m_ObstacleList[i];
		m_ObstacleList[i] = nullptr;
	}
	m_ObstacleList.clear();
}

