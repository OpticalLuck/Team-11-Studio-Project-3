#include "EnemyFactory.h"

//Enemies
#include "../App/Source/Scene2D/MobEnemy2D.h"
#include "../App/Source/Scene2D/Boss2D.h"

CEnemy2D* EnemyFactory::CreateEnemy(int type) {
	switch (type) {
		case 300: {
			CMobEnemy2D* clampedEnemy = new CMobEnemy2D;
			clampedEnemy->SetShader("2DColorShader");
			clampedEnemy->SetClampSlides(true);
			if (clampedEnemy->Init() == false) {
				delete clampedEnemy;
				clampedEnemy = nullptr;
			}

			return clampedEnemy;
		}

		case 301: {
			CMobEnemy2D*enemy = new CMobEnemy2D;
			enemy->SetShader("2DColorShader");
			enemy->SetClampSlides(false);
			if (enemy->Init() == false) {
				delete enemy;
				enemy = nullptr;
			}

			return enemy;
		}

		case 305: {
			CBoss2D* boss = new CBoss2D;
			boss->SetShader("2DColorShader");
			if (!boss->Init()) {
				delete boss;
				boss = nullptr;
			}

			return boss;
		}

		default:
			return nullptr;
	}
}
