#include "EnemyFactory.h"

//Enemies
#include "../App/Source/Scene2D/MobEnemy2D.h"
#include "../App/Source/Scene2D/Boss2D.h"

//Mesh builder
#include "Primitives/MeshBuilder.h"

#include "../Scene2D/Map2D.h"

EnemyFactory::EnemyFactory(void) {
	cSettings = CSettings::GetInstance();
}

CEnemy2D* EnemyFactory::CreateEnemy(int type) {
	switch (type) {
		case 300: {
			// Get the handler to the CMap2D instance
			CMap2D* cMap2D = CMap2D::GetInstance();

			CMobEnemy2D* clampedEnemy = new CMobEnemy2D;
			clampedEnemy->SetID(300);
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
			CMobEnemy2D* enemy = new CMobEnemy2D;
			enemy->SetShader("2DColorShader");
			enemy->SetID(301);
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

		case 302: {
			CBoss2D* mcgun = new CBoss2D;
			mcgun->SetShader("2DColorShader");

			//Custom code for boss type
			std::vector<CBoss2D::ATK> atk;
			atk.push_back(CBoss2D::ATK::A_MACHINEGUN);
			mcgun->SetAtkTypes(atk);
			mcgun->SetPauseEnabled(true);

			mcgun->SetID(302);

			//Changing values of default timers...
			mcgun->SetMinAtkDuration(2.75f);
			mcgun->SetMaxAtkDuration(5.5);

			mcgun->SetMaxPauseDuration(3.5f);

			if (!mcgun->Init()) {
				delete mcgun;
				mcgun = nullptr;
			}
			else {
				mcgun->SetTexture("Image/Enemy/PauseMachineGun.png");
			}

			return mcgun;
		}

		case 303: {
			CBoss2D* mcgun = new CBoss2D;
			mcgun->SetShader("2DColorShader");

			//Custom code for boss type
			std::vector<CBoss2D::ATK> atk;
			atk.push_back(CBoss2D::ATK::A_MACHINEGUN);
			mcgun->SetAtkTypes(atk);
			mcgun->SetPauseEnabled(false);

			mcgun->SetID(303);

			//Changing values of default timers...
			mcgun->SetMinAtkDuration(2.75f);
			mcgun->SetMaxAtkDuration(5.5);

			mcgun->SetMaxPauseDuration(3.5f);

			if (!mcgun->Init()) {
				delete mcgun;
				mcgun = nullptr;
			}
			else {
				mcgun->SetTexture("Image/Enemy/MachineGun.png");
			}

			return mcgun;
		}

		case 304: {
			CMobEnemy2D* enemy = new CMobEnemy2D;
			enemy->SetShader("2DColorShader");
			enemy->SetID(304);

			if (!enemy->Init()) {
				delete enemy;
				enemy = nullptr;
			}
			else {
				//Other values to change from init
				enemy->SetPatrol(true);

				//Initialisation of texture and animation
				enemy->SetTexture("Image/Enemy/pig.png");
				enemy->SetAnimatedSprites(CMeshBuilder::GenerateSpriteAnimation(5, 3, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT));

				enemy->GetAnimatedSprites()->AddAnimation("move", 0, 2);
				enemy->GetAnimatedSprites()->PlayAnimation("move", -1, 0.3f);
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
	return nullptr;
}
