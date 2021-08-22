#include "ObjectFactory.h"

#include "../Scene2D/Boulder2D.h"

#include "../Scene2D/EnemyBullet2D.h"

#include "../Scene2D/Projectiles.h"
#include "../Scene2D/Bullet2D.h"


CObject2D* ObjectFactory::CreateObject(int iTextureID, CLASS_ID eClassID)
{
    CObject2D* newObj = nullptr;
    switch (eClassID)
    {
    case CLASS_ID::CID_COBJECT:
        newObj = new CObject2D(iTextureID);
        break;
    case CLASS_ID::CID_PROJECTILES:
        newObj = new Projectiles(iTextureID);
        break;
    case CLASS_ID::CID_BULLETS:
        newObj = new Bullet2D(iTextureID);
        break;
    case CLASS_ID::CID_BOULDER:
        newObj = new Boulder2D(iTextureID);
        break;
    default:
        newObj = new CObject2D(iTextureID);
        break;
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
