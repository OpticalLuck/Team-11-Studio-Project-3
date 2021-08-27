#pragma once
#include "Projectiles.h"

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

class Bullet2D : public Projectiles
{
public:

	Bullet2D(int iTextureID = OBJECT_TYPE::BULLETS_KUNAI);
	virtual ~Bullet2D();

	bool Init(bool bFriendly, float angle, float force = 10);
	void Update(double dElapsedTime) override;

	void PreRender() override;
	void Render() override;
	void PostRender() override;


	void MapCollision(void);
	void EntityCollision(void);

	void ResolveMapCollision(std::vector<pair<CObject2D*, float>> aabbvector) override;

	void SetFriendly(bool mFriendly);
	bool GetFriendly(void);
private:
	bool mFriendly; //If true, it came from player, if not, enemy
};

