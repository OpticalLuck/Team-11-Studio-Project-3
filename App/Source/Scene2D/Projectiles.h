#pragma once
#include "Object2D.h"

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

class Projectiles : public CObject2D
{
public:
	enum class PROJECTILE_TYPE
	{
		PROJ_SHURIKEN,
		PROJ_TOTAL
	};
	PROJECTILE_TYPE projectileType;

	Projectiles(int iTextureID = OBJECT_TYPE::PROJECTILES_SHURIKEN);
	virtual ~Projectiles();

	bool Init() override;
	void Update(double dElapsedTime) override;

	void PreRender() override;
	void Render() override;
	void PostRender() override;

	bool bOutsideBoundary(void);

	bool bDestroyed;
protected:
	CSpriteAnimation* animatedSprites;

	// Current color
	glm::vec4 currentColor;
};

