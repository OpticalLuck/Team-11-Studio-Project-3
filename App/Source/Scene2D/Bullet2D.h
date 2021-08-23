#pragma once
#include "Physics2D.h"
#include "Object2D.h"

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

class Bullet2D : public CObject2D
{
public:

	Bullet2D(int iTextureID = OBJECT_TYPE::BULLETS_KUNAI);
	virtual ~Bullet2D();

	bool Init() override;
	void Update(double dElapsedTime) override;

	void PreRender() override;
	void Render() override;
	void PostRender() override;
	CPhysics2D& GetPhysics();

private:
	CSpriteAnimation* animatedSprites;

	// Current color
	glm::vec4 currentColor;

	CPhysics2D cPhysics2D;
};

