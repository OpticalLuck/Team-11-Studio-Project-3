#include "Item.h"
#include "Object2D.h"
#include "../Factory/ObjectFactory.h"

#include "../Scene2D/Player2D.h"
#include "../Scene2D/Bullet2D.h"
#include "../Scene2D/Projectiles.h"
#include "../Scene2D/EntityManager.h"

CItem::CItem(int ID)
	: iID(ID)
{
}

CItem::CItem(int ID, std::string _name)
	: iID(ID)
	, sName(_name)
{
}

CItem::CItem()
{
}

CItem::~CItem()
{
}

int CItem::get_ID()
{
	return iID;
}

int CItem::set_ID(int _ID)
{
	iID = _ID;
	return iID;
}

std::string CItem::GetName()
{
	return sName;
}

void CItem::set_Name(std::string _name)
{
	sName = _name;
}

void CItem::Use(CPlayer2D* user)
{
	if (iCount > iMinCount)
	{
		iCount--;
		switch (iID)
		{
		case OBJECT_TYPE::CONSUMABLES_POTION:
			cout << "You are using potion\n";
			cout << "iCount is " << iCount << '\n';
			break;
		case OBJECT_TYPE::PROJECTILES_SHURIKEN:
		{
			glm::vec2 distance = user->m_MouseInputs[user->iTempFrameCounter][MOUSE_INPUTS::LMB].vMousePos - user->vTransform;

			CObject2D* shurikenobj = ObjectFactory::CreateObject(OBJECT_TYPE::PROJECTILES_SHURIKEN);
			shurikenobj->Init();
			shurikenobj->vTransform = user->vTransform;

			glm::vec2 force = glm::clamp(distance * 200.f, glm::vec2(-2000.f, -2000.f), glm::vec2(2000.f, 2000.f));
			shurikenobj->GetPhysics()->SetForce(force);

			CEntityManager::GetInstance()->PushBullet(static_cast<Projectiles*>(shurikenobj));

			cout << "You are using Item shuriken\n";
			cout << "iCount is " << iCount << '\n';
			break;
		}
		case OBJECT_TYPE::BULLETS_KUNAI:
		{
			CObject2D* kunaiobj = ObjectFactory::CreateObject(OBJECT_TYPE::BULLETS_KUNAI);

			float angle = 0;
			if (user->facing == CPlayer2D::FACING_DIR::LEFT)
				angle = 180;
			else
				angle = 0;

			dynamic_cast<Bullet2D*>(kunaiobj)->Init(true, angle, 10);
			kunaiobj->vTransform = user->vTransform;

			CEntityManager::GetInstance()->PushBullet(static_cast<Bullet2D*>(kunaiobj));
			cout << "You are using Item kunai\n";
			cout << "iCount is " << iCount << '\n';
			break;
		}
		}
	}
}



