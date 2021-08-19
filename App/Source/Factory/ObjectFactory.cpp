#include "ObjectFactory.h"

#include "../Scene2D/Boulder2D.h"

CObject2D* ObjectFactory::CreateObject(int id)
{
	if (id >= 150 && id < 160)
		return new Boulder2D();
	else
		return new CObject2D();

}
