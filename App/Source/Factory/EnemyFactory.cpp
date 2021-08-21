#include "EnemyFactory.h"

//Enemies
#include "../App/Source/Scene2D/MobEnemy2D.h"
#include "../App/Source/Scene2D/Boss2D.h"

//Mesh builder
#include "Primitives/MeshBuilder.h"

EnemyFactory::EnemyFactory(void) {
	cSettings = CSettings::GetInstance();
}

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
			else {
				//Initialisation of texture and animation
				clampedEnemy->SetTexture("Image/Enemy/Mushroom.png");
				clampedEnemy->SetAnimatedSprites(CMeshBuilder::GenerateSpriteAnimation(4, 3, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT));

				//Creating animation... "idle", "move", "attack"
				CSpriteAnimation* animation = clampedEnemy->GetAnimatedSprites();
				animation->AddAnimation("move", 0, 2);
				animation->PlayAnimation("move", -1, 0.6f);
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
			else {
				//Initialisation of texture and animation
				enemy->SetTexture("Image/Enemy/Snail.png");
				enemy->SetAnimatedSprites(CMeshBuilder::GenerateSpriteAnimation(3, 9, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT));

				//Creating animation... "idle", "move", "attack"
				CSpriteAnimation* animation = enemy->GetAnimatedSprites();
				animation->AddAnimation("move", 0, 6);
				animation->PlayAnimation("move", -1, 1.f);
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
			else {
				boss->SetTexture("Image/Scene2D_EnemyTile.tga");
			}

			return boss;
		}

		default:
			return nullptr;
	}
}
