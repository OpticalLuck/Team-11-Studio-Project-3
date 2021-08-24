#pragma once

#include "../Scene2D/Object2D.h"

class Bullet2D;

class ObjectFactory
{
public:
	static CObject2D* CreateObject(int iTextureID);
	static Bullet2D* CreateBullet(float angle, glm::vec2 vTransform, bool friendly = false);
};

