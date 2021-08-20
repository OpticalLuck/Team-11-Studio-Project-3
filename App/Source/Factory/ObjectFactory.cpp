#include "ObjectFactory.h"

#include "../Scene2D/Boulder2D.h"
#include "../App/Source/Scene2D/EnemyBullet2D.h"

CObject2D* ObjectFactory::CreateObject(int id)
{
	if (id >= 150 && id < 160)
		return new Boulder2D();
	else
		return new CObject2D();

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
