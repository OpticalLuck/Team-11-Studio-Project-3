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
	if (!cPhysics2D)
		cPhysics2D = new CPhysics2D;
	if (!collider2D)
		collider2D = new Collider2D;

	cPhysics2D->Init(&vTransform);
	cPhysics2D->SetMass(10);

	collider2D->Init(vTransform, glm::vec2(0.5f), Collider2D::ColliderType::COLLIDER_CIRCLE);
	return true;
}

void Boulder2D::Update(const double dElapsedTime)
{
	cPhysics2D->Update(dElapsedTime);
	// Update Collider2D Position
	collider2D->SetPosition(vTransform);

	CMap2D* cMap2D = CMap2D::GetInstance();

	ResolveMapCollision(CheckMapCollision());

	//Update Map index
	glm::i32vec2 newindex((int)vTransform.x, CMap2D::GetInstance()->GetLevelRow() - (int)vTransform.y - 1);
	if (newindex != currentIndex)
	{
		CMap2D::GetInstance()->UpdateGridInfo(newindex.y, newindex.x, this, false);
	}

}

void Boulder2D::ResolveMapCollision(std::vector<pair<CObject2D*, float>> aabbvector)
{
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

			if (obj->type == ENTITY_TYPE::TILE)
			{
				if (dynamic_cast<Boulder2D*>(obj))
				{
					glm::vec2 direction = glm::normalize(obj->vTransform - vTransform);
					obj->GetPhysics()->SetForce(glm::vec2(120.f, 0) * direction);
					cPhysics2D->SetVelocity(glm::vec2(0.f));
				}
			}

			vTransform = collider2D->position;
			obj->vTransform = obj->GetCollider()->position;
		}
	}
}
