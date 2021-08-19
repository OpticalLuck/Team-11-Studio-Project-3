#pragma once
#include "Physics2D.h"
#include "Object2D.h"

#include "..\App\Source\Command\Command.h"

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

class Projectiles : public CObject2D
{
public:
	enum PROJECTILE_TYPE
	{
		PROJ_SHURIKEN,
		PROJ_TOTAL
	};
	PROJECTILE_TYPE projectileType;

	Projectiles();
	virtual ~Projectiles();

	bool Init() override;
	void Update(double dElapsedTime) override;

	CPhysics2D& GetPhysics();
protected:

	CSpriteAnimation* animatedSprites;

	// Current color
	glm::vec4 currentColor;

	// Command to run when interacted with
	Command cCommand;

	CPhysics2D cPhysics2D;
};

