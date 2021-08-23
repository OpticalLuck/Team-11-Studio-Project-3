#pragma once

#include "../Scene2D/Object2D.h"

class EnemyBullet2D;


class ObjectFactory
{
public:
	static CObject2D* CreateObject(int iTextureID);
	static EnemyBullet2D* CreateBullet(float angle, glm::vec2 vTransform);
};

