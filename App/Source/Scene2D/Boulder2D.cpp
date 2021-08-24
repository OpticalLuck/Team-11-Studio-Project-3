#include "Boulder2D.h"
#include "Map2D.h"

Boulder2D::Boulder2D(int iTextureID)
{
	this->iTextureID = iTextureID;
	type = ENTITY_TYPE::TILE;
}

Boulder2D::~Boulder2D()
{
}

bool Boulder2D::Init()
{
	cPhysics2D.Init(&vTransform);
	cPhysics2D.SetMass(10);

	collider2D->Init(vTransform, glm::vec2(0.5f), Collider2D::ColliderType::COLLIDER_CIRCLE);
	return true;
}

void Boulder2D::Update(const double dElapsedTime)
{
	cPhysics2D.Update(dElapsedTime);
	// Update Collider2D Position
	collider2D->SetPosition(vTransform);

	CMap2D* cMap2D = CMap2D::GetInstance();

	int range = 2;
	//Stores nearby objects and its dist to player into a vector 
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
	// Detects and Resolves Collsion
	for (auto aabbTuple : aabbVector)
	{
		CObject2D* obj = aabbTuple.first;
		Collision data = (collider2D->CollideWith(obj->GetCollider()));
		if (std::get<0>(data))
		{
			if (obj->GetCollider()->colliderType == Collider2D::ColliderType::COLLIDER_QUAD)
			{
				collider2D->ResolveAABB(obj->GetCollider(), data);

				if (std::get<1>(data) == Direction::UP)
					cPhysics2D.SetboolGrounded(true);
			}
			else if (obj->GetCollider()->colliderType == Collider2D::ColliderType::COLLIDER_CIRCLE)
			{
				if (glm::dot(cPhysics2D.GetVelocity(), obj->vTransform - vTransform) > 0)
					collider2D->ResolveAABBCircle(obj->GetCollider(), data, Collider2D::ColliderType::COLLIDER_QUAD);

				if (std::get<1>(data) == Direction::DOWN)
					cPhysics2D.SetboolGrounded(true);
			}

			vTransform = collider2D->position;
			obj->vTransform = obj->GetCollider()->position;

			if (obj->type == ENTITY_TYPE::TILE)
			{
				if (dynamic_cast<Boulder2D*>(obj))
				{
					glm::vec2 direction = glm::normalize(obj->vTransform - vTransform);
					static_cast<Boulder2D*>(obj)->GetPhysics().SetForce(glm::vec2(120.f, 0) * direction);
					cPhysics2D.SetVelocity(glm::vec2(0.f));
				}
			}
		}
	}

	vTransform = collider2D->position;

	//Update Map index
	glm::i32vec2 newindex((int)vTransform.x, CMap2D::GetInstance()->GetLevelRow() - (int)vTransform.y - 1);
	if (newindex != currentIndex)
	{
		CMap2D::GetInstance()->UpdateGridInfo(newindex.y, newindex.x, this, false);
	}

}

CPhysics2D& Boulder2D::GetPhysics()
{
	// TODO: insert return statement here
	return cPhysics2D;
}
