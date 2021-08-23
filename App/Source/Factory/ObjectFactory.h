#pragma once

#include "../Scene2D/Object2D.h"

class EnemyBullet2D;

enum class CLASS_ID
{
	CID_NONE		= 0,
	CID_COBJECT		= 1,
	CID_PROJECTILES = 2,
	CID_BULLETS		= 3,
	CID_BOULDER		= 4,
	CID_TOTAL
};

class ObjectFactory
{
public:
	CObject2D* CreateObject(int iTextureID);
	EnemyBullet2D* CreateBullet(float angle, glm::vec2 vTransform);
};

