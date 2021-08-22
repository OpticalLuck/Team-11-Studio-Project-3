#include "Bullet2D.h"

#include "Map2D.h"
#include <vector>
Bullet2D::Bullet2D()
	: animatedSprites(NULL)
	, currentColor(glm::vec4())
{
	value = OBJECT_TYPE::ITEM_KUNAI; //Shuriken also by default
}

Bullet2D::~Bullet2D()
{
}

bool Bullet2D::Init()
{
	cPhysics2D.Init(&vTransform);
	cPhysics2D.MAX_SPEED = 50.f;
	cPhysics2D.FRICTONAL_COEFFICIENT = 0.8f;
	collider2D->colliderType = Collider2D::ColliderType::COLLIDER_CIRCLE;
	collider2D->position = vTransform;
	collider2D->vec2Dimensions = glm::vec2(0.2f);
	collider2D->Init();

	cPhysics2D.SetGravity(0.f);
	return false;
}

void Bullet2D::Update(double dElapsedTime)
{
	cPhysics2D.Update(dElapsedTime);
	collider2D->SetPosition(vTransform);

	CMap2D* cMap2D = CMap2D::GetInstance();

	if (cPhysics2D.GetVelocity().x < 0)
		vRotate = 180;
	else
		vRotate = 0;

	int range = 2;
	cPhysics2D.SetboolGrounded(false);
	vector<pair<CObject2D*, float>> aabbVector;
	for (int row = -range; row <= range; row++) //y
	{
		for (int col = -range; col <= range; col++) //x
		{
			int rowCheck = vTransform.y + row;
			int colCheck = vTransform.x + col;

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
	bool destroyed = false;
	for (auto aabbTuple : aabbVector)
	{
		CObject2D* obj = aabbTuple.first;
		Collision data = (collider2D->CollideWith(obj->GetCollider()));
		if (std::get<0>(data))
		{
			destroyed = true;
		}
	}



	//Update Map index
	glm::i32vec2 newindex((int)vTransform.x, CMap2D::GetInstance()->GetLevelRow() - (int)vTransform.y - 1);
	if (newindex != currentIndex)
	{
		CMap2D::GetInstance()->UpdateGridInfo(newindex.y, newindex.x, this, false);
	}
	if (destroyed)
	{
		cMap2D->SetMapInfo(currentIndex.y, currentIndex.x, 0, false);
	}
}

CPhysics2D& Bullet2D::GetPhysics()
{
	return cPhysics2D;
}