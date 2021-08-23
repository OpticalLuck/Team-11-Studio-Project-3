#pragma once
#include "Physics2D.h"
#include "Object2D.h"

#include "..\App\Source\Command\Command.h"

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"
class Bullet2D : public CObject2D
{
public:

	Bullet2D(int iTextureID = OBJECT_TYPE::ITEM_KUNAI);
	virtual ~Bullet2D();

	bool Init() override;
	void Update(double dElapsedTime) override;

	CPhysics2D& GetPhysics();

private:
	CSpriteAnimation* animatedSprites;

	// Current color
	glm::vec4 currentColor;

	// Command to run when interacted with
	Command cCommand;

	CPhysics2D cPhysics2D;
};

