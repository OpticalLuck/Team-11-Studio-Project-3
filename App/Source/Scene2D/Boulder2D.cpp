#include "Boulder2D.h"
#include "Map2D.h"

Boulder2D::Boulder2D(int iTextureID)
{
	this->iTextureID = iTextureID;
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
	for (int i = 0; i < 2; i++)
	{
		for (int row = -range; row <= range; row++) //y
		{
			for (int col = -range; col <= range; col++) //x
			{
				int rowCheck = (int)vTransform.y + row;
				int colCheck = (int)vTransform.x + col;

				if (rowCheck < 0 || colCheck < 0 || rowCheck > cMap2D->GetLevelRow() - 1 || colCheck > cMap2D->GetLevelCol() - 1)
					continue;

				if (cMap2D->GetCObject(colCheck, rowCheck) && cMap2D->GetCObject(colCheck, rowCheck) != this)
				{
					CObject2D* obj = cMap2D->GetCObject(colCheck, rowCheck);
					Collision data = (collider2D->CollideWith(cMap2D->GetCObject(colCheck, rowCheck)->GetCollider()));
					if (std::get<0>(data))
					{
						if (obj->GetCollider()->colliderType == Collider2D::ColliderType::COLLIDER_QUAD)
						{
							if (i == 0)
								collider2D->ResolveAABB(obj->GetCollider(), Direction::UP);
							else if (i == 1)
								collider2D->ResolveAABB(obj->GetCollider(), Direction::RIGHT);

						}
						
					}
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
