#pragma once

#include "../App/Source/Scene2D/Enemy2D.h"

class EnemyFactory
{
	public:
		//Constructor
		EnemyFactory(void);

		CEnemy2D* CreateEnemy(int type);

	protected:
		//Handlers
		CSettings* cSettings;
};

