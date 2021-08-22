#include "ObjectFactory.h"

#include "../Scene2D/Boulder2D.h"

#include "../App/Source/Scene2D/EnemyBullet2D.h"

#include "../Scene2D/Projectiles.h"

CObject2D* ObjectFactory::CreateObject(int id)
{
	CObject2D* newobj = nullptr;
	if (id >= 150 && id < 160)
	{
		CObject2D* newobj = new Boulder2D();
		newobj->SetTextureID(id);
		return newobj;
	}
	else if (id >= 2 && id < 10)
	{
		CObject2D* newobj = new Projectiles();
		newobj->SetTextureID(id);
		return newobj;
	}
	else
	{
		CObject2D* newobj = new CObject2D();
		newobj->SetTextureID(id);
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
