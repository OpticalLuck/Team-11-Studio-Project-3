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

	//// PreRender
	//void PreRender(void);

	//// Render
	//void Render(void);

	//// PostRender
	//void PostRender(void);

	CPhysics2D& GetPhysics();
protected:
	CPhysics2D cPhysics2D;

};

