#pragma once

#include "../App/Source/Scene2D/Enemy2D.h"

class EnemyFactory
{
	public:
		CEnemy2D* CreateEnemy(int type);
};

