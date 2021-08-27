#pragma once
#include "Interactables.h"

class Boulder2D : public CObject2D
{
public:
	Boulder2D(int iTextureID = OBJECT_TYPE::OBSTACLE_BOULDER);
	virtual ~Boulder2D();

	bool Init() override;
	void Update(const double dElapsedTime) override;

	void ResolveMapCollision(std::vector<pair<CObject2D*, float>> aabbvector) override;
protected:
};

