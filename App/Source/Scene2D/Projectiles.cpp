#include "Projectiles.h"

#include "Map2D.h"
#include <vector>

Projectiles::Projectiles(int iTextureID)
	: animatedSprites(NULL)
	, currentColor(glm::vec4())
{
	projectileType = PROJ_SHURIKEN;
	this->iTextureID = iTextureID;
}

Projectiles::~Projectiles()
{
	if (animatedSprites)
		delete animatedSprites;
}

bool Projectiles::Init()
{
	cPhysics2D.Init(&vTransform);
	cPhysics2D.MAX_SPEED = 50.f;
	cPhysics2D.FRICTONAL_COEFFICIENT = 0.8f;

	collider2D->Init(vTransform, glm::vec2(0.2f), Collider2D::ColliderType::COLLIDER_CIRCLE);

	return false;
}

void Projectiles::Update(double dElapsedTime)
{
	cPhysics2D.Update(dElapsedTime);
	// Update Collider2D Position
	collider2D->SetPosition(vTransform);

	CMap2D* cMap2D = CMap2D::GetInstance();

	bool physicsChange = false;

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
			if (obj->GetCollider()->colliderType == Collider2D::COLLIDER_QUAD)
			{
				collider2D->ResolveAABB(obj->GetCollider(), Direction::UP);
				collider2D->ResolveAABB(obj->GetCollider(), Direction::RIGHT);

				if (std::get<1>(data) == Direction::UP)
					cPhysics2D.SetboolGrounded(true);
			}
			else if (obj->GetCollider()->colliderType == Collider2D::COLLIDER_CIRCLE)
			{
				if (glm::dot(cPhysics2D.GetVelocity(), obj->vTransform - vTransform) > 0)
					collider2D->ResolveAABBCircle(obj->GetCollider(), data, Collider2D::COLLIDER_QUAD);

				if (std::get<1>(data) == Direction::DOWN)
					cPhysics2D.SetboolGrounded(true);
			}

			vTransform = collider2D->position;

			//How the object interacts 
			glm::vec2 normal = Collider2D::ConvertDirectionToVec2(std::get<1>(data));
			cPhysics2D.DoBounce(normal, 0.f);
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

CPhysics2D& Projectiles::GetPhysics()
{
	return cPhysics2D;
}
