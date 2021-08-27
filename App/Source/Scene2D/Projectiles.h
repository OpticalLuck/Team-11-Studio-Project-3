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

	virtual bool Init() override;
	virtual void Update(double dElapsedTime);

	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;

	bool bOutsideBoundary(void);

	void ResolveEnemyCollision() override;

	bool bDestroyed;
protected:
	CSpriteAnimation* animatedSprites;

	// Current color
	int iBounces;
	glm::vec4 currentColor;
};

