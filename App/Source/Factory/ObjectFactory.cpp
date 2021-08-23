#include "ObjectFactory.h"

#include "../Scene2D/Boulder2D.h"
#include "../Scene2D/Projectiles.h"
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
		CObject2D* newobj = new Projectiles();
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
