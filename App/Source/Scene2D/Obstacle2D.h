#pragma once
#include "Interactables.h"

class Obstacle2D : public CObject2D
{
public:
	Obstacle2D(int iTextureID = OBJECT_TYPE::OBSTACLE_BOULDER);
	virtual ~Obstacle2D();

	bool Init() override;
	void Update(const double dElapsedTime) override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;

	void ResolveMapCollision(std::vector<pair<CObject2D*, float>> aabbvector) override;
	void ResolvePlayerCollision() override;
	void ResolveEnemyCollision() override;

protected:
	float force;
};

