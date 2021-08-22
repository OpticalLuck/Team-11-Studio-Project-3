#pragma once
#include "Physics2D.h"
#include "Interactables.h"

class Boulder2D : public CObject2D
{
public:
	Boulder2D(int iTextureID = OBJECT_TYPE::OBSTACLE_BOULDER);
	virtual ~Boulder2D();

	bool Init() override;
	void Update(const double dElapsedTime) override;

	CPhysics2D& GetPhysics();
protected:
	CPhysics2D cPhysics2D;
};

