#pragma once

#include "../Scene2D/Object2D.h"

class EnemyBullet2D;


class ObjectFactory
{
public:
	CObject2D* CreateObject(int iTextureID);
	EnemyBullet2D* CreateBullet(float angle, glm::vec2 vTransform);
};

