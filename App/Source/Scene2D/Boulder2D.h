#pragma once
#include "Physics2D.h"
#include "Interactables.h"

class Boulder2D : public Interactables
{
public:
	Boulder2D();
	virtual ~Boulder2D();

	bool Init() override;
	void Update(const double dElapsedTime) override;

	CPhysics2D& GetPhysics();
protected:
	CPhysics2D cPhysics2D;

};

