#include "ObjectFactory.h"

#include "../Scene2D/Boulder2D.h"

#include "../Scene2D/EnemyBullet2D.h"

#include "../Scene2D/Projectiles.h"
#include "../Scene2D/Bullet2D.h"

CObject2D* ObjectFactory::CreateObject(int id)
{
	CObject2D* newobj = nullptr;
	if (id >= 150 && id < 160)
	{
		CObject2D* newobj = new Boulder2D();
		newobj->SetValue(id);
		return newobj;
	}
	else if (id >= 2 && id < 10)
	{
		CObject2D* newobj = nullptr;
		if(id == OBJECT_TYPE::ITEM_SHURIKEN)
			newobj = new Projectiles();
		else if (id == OBJECT_TYPE::ITEM_KUNAI)
			newobj = new Bullet2D();
		else
			newobj = new CObject2D();

		newobj->SetValue(id);
		return newobj;
	}
	else
	{
		CObject2D* newobj = new CObject2D();
		newobj->SetValue(id);
		return newobj;
	}

}

EnemyBullet2D* ObjectFactory::CreateBullet(float angle, glm::vec2 vTransform) {
	EnemyBullet2D* bullet = new EnemyBullet2D;
	if (!bullet->Init(angle, vTransform)) {
		delete bullet;
		bullet = nullptr; //Delete bullet if initialisation fails
	}
	else {
		bullet->SetShader("2DColorShader");
	}

	return bullet;
}
