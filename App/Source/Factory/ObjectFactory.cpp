#include "ObjectFactory.h"

#include "../Scene2D/Boulder2D.h"

#include "../Scene2D/Bullet2D.h"

#include "../App/Source/TextureManager/TextureManager.h"

#include "System/Debug.h"

CObject2D* ObjectFactory::CreateObject(int iTextureID)
{
    CObject2D* newObj = nullptr;

	if (iTextureID > TILE_START && iTextureID < TILE_TOTAL)
	{
		newObj = new CObject2D(iTextureID);
	}
	else if (iTextureID > CONSUMABLES_START && iTextureID < CONSUMABLES_TOTAL)
	{
		newObj = new CObject2D(iTextureID);
	}
	else if (iTextureID > INTERACTABLE_START && iTextureID < INTERACTABLE_TOTAL)
	{
		newObj = new Interactables(iTextureID);
	}
	else if (iTextureID > PROJECTILES_START && iTextureID < PROJECTILES_TOTAL)
	{
		newObj = new Projectiles(iTextureID);
	}
	else if (iTextureID > BULLETS_START && iTextureID < BULLETS_TOTAL)
	{
		newObj = new Bullet2D(iTextureID);
	}
	else if (iTextureID > OBSTACLES_START && iTextureID < OBSTACLES_TOTAL)
	{
		if(iTextureID == OBSTACLE_BOULDER)
			newObj = new Boulder2D(iTextureID);
		else
			newObj = new CObject2D(iTextureID);
	}
	else
		newObj = new CObject2D(iTextureID);

    return newObj;
}

Bullet2D* ObjectFactory::CreateBullet(float angle, glm::vec2 vTransform, bool friendly) {
	Bullet2D* bullet = dynamic_cast<Bullet2D*>(ObjectFactory::CreateObject(BULLETS_ENEMY));
	if (!bullet->Init(friendly, angle)) {
		delete bullet;
		bullet = nullptr; //Delete bullet if initialisation fails
		DEBUG_MSG("BULLET FAILED TO BE CREATED FROM OBJECT FACTORY");
	}
	else {
		bullet->SetShader("2DColorShader");
		bullet->vTransform = vTransform;
	}

	return bullet;
}
