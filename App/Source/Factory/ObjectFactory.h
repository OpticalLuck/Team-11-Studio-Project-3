#pragma once

#include "../Scene2D/Object2D.h"

class EnemyBullet2D;

enum class CLASS_ID
{
	CID_COBJECT		= 0,
	CID_PROJECTILES = 1,
	CID_BULLETS		= 2,
	CID_BOULDER		= 3,
	CID_TOTAL
};

class ObjectFactory
{
public:
	CObject2D* CreateObject(int iTextureID, CLASS_ID eClassID);
	EnemyBullet2D* CreateBullet(float angle, glm::vec2 vTransform);
};

