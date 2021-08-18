#include "Boulder2D.h"
#include "Map2D.h"

Boulder2D::Boulder2D()
{
	interactableType = Interactables::BOULDER;
}

Boulder2D::~Boulder2D()
{
}

bool Boulder2D::Init()
{
	//Collider2D initialisation
	collider2D->Init();
	collider2D->position = vTransform + glm::vec2(0.5f, 0.5f);

	if (value >= 100 && value < 300)
		collider2D->SetbEnabled(true);
	else
		collider2D->SetbEnabled(false);

	cPhysics2D.Init(&vTransform);
	cPhysics2D.SetMass(10);
	return true;
}

void Boulder2D::Update(const double dElapsedTime)
{
	cPhysics2D.Update(dElapsedTime);
	// Update Collider2D Position
	collider2D->SetPosition(vTransform);

	CMap2D* cMap2D = CMap2D::GetInstance();

	int range = 2;
	//y
	for (int row = -range; row <= range; row++)
	{
		//x
		for (int col = -range; col <= range; col++)
		{
			int rowCheck = vTransform.y + row;
			int colCheck = vTransform.x + col;

			if (rowCheck < 0 || colCheck < 0 || rowCheck > cMap2D->GetLevelRow() - 1 || colCheck > cMap2D->GetLevelCol() - 1)
				continue;

			if (cMap2D->GetCObject(colCheck, rowCheck) && cMap2D->GetCObject(colCheck, rowCheck) != this)
			{
				if (collider2D->CollideWith(cMap2D->GetCObject(colCheck, rowCheck)->GetCollider()))
				{
					//Collider2D::CorrectedAxis axis = collider2D.ResolveCollision(cMap2D->GetCollider(uiRow, uiCol));
					collider2D->ResolveCollisionX(cMap2D->GetCObject(colCheck, rowCheck)->GetCollider());
					// Resolve transform to corrected position in collider
					vTransform = collider2D->position;
				}
			}
		}
	}

	//COLLISION RESOLUTION ON X_AXIS
	for (int row = -range; row <= range; row++)
	{
		for (int col = -range; col <= range; col++)
		{
			int rowCheck = vTransform.y + row;
			int colCheck = vTransform.x + col;

			if (rowCheck < 0 || colCheck < 0 || rowCheck > cMap2D->GetLevelRow() - 1 || colCheck > cMap2D->GetLevelCol() - 1)
				continue;

			if (cMap2D->GetCObject(colCheck, rowCheck) && cMap2D->GetCObject(colCheck, rowCheck) != this)
			{
				if (collider2D->CollideWith(cMap2D->GetCObject(colCheck, rowCheck)->GetCollider()))
				{
					cPhysics2D.SetVelocity(glm::vec2(cPhysics2D.GetVelocity().x, 0));
					collider2D->ResolveCollisionY(cMap2D->GetCObject(colCheck, rowCheck)->GetCollider());
					// Resolve transform to corrected position in collider
					vTransform = collider2D->position;
				}
			}
		}
	}
}

CPhysics2D& Boulder2D::GetPhysics()
{
	// TODO: insert return statement here
	return cPhysics2D;
}
