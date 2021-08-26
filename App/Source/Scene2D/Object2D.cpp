#include "Object2D.h"
#include <iostream>

#include "EntityManager.h"
#include "MobEnemy2D.h"

CObject2D::CObject2D(int iTextureID) 
	: width(1.f)
	, height(1.f)
{
	this->iTextureID = iTextureID;

	vTransform = glm::i32vec2();
	currentIndex = glm::i32vec2();

	cEntityManager = nullptr;
}

CObject2D::~CObject2D(void) {
	cEntityManager = nullptr; //Dellocation of handlers
}

bool CObject2D::Init()
{
	if (!collider2D)
		collider2D = new Collider2D;
	if (!cPhysics2D)
		cPhysics2D = new CPhysics2D;

	//Collider2D initialisation
	collider2D->Init(vTransform);
	collider2D->SetbEnabled(true);

	cEntityManager = CEntityManager::GetInstance();

	return true;
}

void CObject2D::Update(const double dElapsedTime)
{
	if (collider2D->position != vTransform)
		collider2D->position = vTransform;
}

int CObject2D::GetObjectID(void) {
	return ObjectID;
}

glm::i32vec2 CObject2D::GetCurrentIndex() const
{
	return currentIndex;
}

void CObject2D::SetCurrentIndex(glm::i32vec2 currentIndex)
{
	this->currentIndex = currentIndex;
}

void CObject2D::SetObjectID(int objectID) {
	this->ObjectID = objectID;
}

std::vector<pair<CObject2D*, float>> CObject2D::CheckMapCollision(int range)
{
	CMap2D* cMap2D = CMap2D::GetInstance();

	cPhysics2D->SetboolGrounded(false);
	vector<pair<CObject2D*, float>> aabbVector;
	for (int row = -range; row <= range; row++) //y
	{
		for (int col = -range; col <= range; col++) //x
		{
			int rowCheck = (int)vTransform.y + row;
			int colCheck = (int)vTransform.x + col;

			if (rowCheck < 0 || colCheck < 0 || rowCheck > cMap2D->GetLevelRow() - 1 || colCheck > cMap2D->GetLevelCol() - 1) continue;

			if (cMap2D->GetCObject(colCheck, rowCheck) && cMap2D->GetCObject(colCheck, rowCheck) != this)
			{
				CObject2D* obj = cMap2D->GetCObject(colCheck, rowCheck);
				float distance = glm::length(obj->vTransform - vTransform);
				aabbVector.push_back({ obj, distance });
			}
		}
	}
	//Sorts vector based on shortest dist from player to object
	sort(aabbVector.begin(), aabbVector.end(), [](const std::pair<CObject2D*, float>& a, const std::pair<CObject2D*, float>& b)
		{
			return a.second < b.second;
		});
	aabbVector.erase(std::unique(aabbVector.begin(), aabbVector.end()), aabbVector.end());

	return aabbVector;
}

void CObject2D::ResolveEnemyCollision()
{
	std::vector<CEnemy2D*> enemyArr = cEntityManager->GetAllEnemies();

	//collider2D->CollideWith(obj->GetCollider())
	for (unsigned i = 0; i < enemyArr.size(); i++) {
		Collision data = collider2D->CollideWith(enemyArr[i]->GetCollider());

		if (std::get<0>(data)) { //If collided with 
			//Enemy collision code activate!
			CMobEnemy2D* mobEnemy = dynamic_cast<CMobEnemy2D*>(enemyArr[i]);

			if (mobEnemy)
				mobEnemy->Attacked(1, cPhysics2D);
			else
				enemyArr[i]->Attacked();
		}
	}
}

void CObject2D::ResolvePlayerCollision()
{
	//Player collision
	std::vector<CPlayer2D*> arrPlayer = cEntityManager->GetAllPlayers();

	for (unsigned i = 0; i < arrPlayer.size(); i++) {
		Collider2D* playerCollider = arrPlayer[i]->GetCollider();
		Collision data = (collider2D->CollideWith(playerCollider));

		if (std::get<0>(data)) {
			//Player collision code below
			arrPlayer[i]->Attacked();

			return;
		}
	}
}

void CObject2D::ResolveMapCollision(std::vector<pair<CObject2D*, float>> aabbvector)
{
	bool destroyed = false;
	for (auto aabbTuple : aabbvector)
	{
		CObject2D* obj = aabbTuple.first;
		Collision data = (collider2D->CollideWith(obj->GetCollider()));
		if (std::get<0>(data))
		{
			if (obj->GetCollider()->colliderType == Collider2D::ColliderType::COLLIDER_QUAD)
			{
				collider2D->ResolveAABB(obj->GetCollider(), data);

				if (std::get<1>(data) == Direction::UP)
					cPhysics2D->SetboolGrounded(true);
			}
			else if (obj->GetCollider()->colliderType == Collider2D::ColliderType::COLLIDER_CIRCLE)
			{
				if (glm::dot(cPhysics2D->GetVelocity(), obj->vTransform - vTransform) > 0)
					collider2D->ResolveAABBCircle(obj->GetCollider(), data, Collider2D::ColliderType::COLLIDER_QUAD);

				if (std::get<1>(data) == Direction::DOWN)
					cPhysics2D->SetboolGrounded(true);
			}

			vTransform = collider2D->position;

			//How the object interacts 
			glm::vec2 normal = Collider2D::ConvertDirectionToVec2(std::get<1>(data));
			cPhysics2D->DoBounce(normal, 1.f);
		}
	}
}


