#include "Projectiles.h"

#include "Map2D.h"
#include <vector>

#include "EntityManager.h"
#include "Primitives/MeshBuilder.h"
#include "RenderControl/ShaderManager.h"
#include "../TextureManager/TextureManager.h"
#include "Primitives/Camera2D.h"

#include "MobEnemy2D.h"

Projectiles::Projectiles(int iTextureID)
	: animatedSprites(NULL)
	, currentColor(glm::vec4())
	, bDestroyed(false)
	, iBounces(0)
	, fusetimer(5)
{
	projectileType = PROJECTILE_TYPE::PROJ_SHURIKEN;
	this->iTextureID = iTextureID;
}

Projectiles::~Projectiles()
{
	if (animatedSprites)
		delete animatedSprites;
}

bool Projectiles::Init()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	cSettings = CSettings::GetInstance();
	mesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	cSettings = CSettings::GetInstance();


	if (!cPhysics2D)
		cPhysics2D = new CPhysics2D;
	if (!collider2D)
		collider2D = new Collider2D;

	cPhysics2D->Init(&vTransform);
	cPhysics2D->MAX_SPEED = 50.f;
	cPhysics2D->FRICTONAL_COEFFICIENT = 1.f;
	cPhysics2D->SetMass(10.f);

	collider2D->Init(vTransform, glm::vec2(0.4f), Collider2D::ColliderType::COLLIDER_CIRCLE);

	cEntityManager = CEntityManager::GetInstance();

	return false;
}

void Projectiles::Update(double dElapsedTime)
{
	if (iBounces > 0)
		fusetimer -= dElapsedTime;

	
	cPhysics2D->Update(dElapsedTime);

	// Update Collider2D Position
	collider2D->SetPosition(vTransform);

	//Collision between projectile and enemy
	//ResolveEnemyCollision();
	if (fusetimer < 0)
	{
		bDestroyed = true;
		CSoundController::GetInstance()->PlaySoundByID(SOUND_ID::SOUND_EXPLOSION);

		CEntityManager* em = CEntityManager::GetInstance();
		for (auto& e : em->GetAllEnemies())
		{
			if (glm::distance(vTransform, e->vTransform) < 3)
			{
				CMobEnemy2D* mobEnemy = dynamic_cast<CMobEnemy2D*>(e);

				if (mobEnemy)
				{
					mobEnemy->Attacked(1, cPhysics2D);
				}
				else
				{
					e->Attacked();
				}
			}
		}

		CPlayer2D* player = em->GetPlayer();
		if (glm::distance(vTransform, player->vTransform) < 2)
		{
			player->Attacked(2, cPhysics2D);
		}
	}

	//Collision between objects in map space
	ResolveMapCollision(CheckMapCollision());

	if (bCollided)
	{
		iBounces++;
	}
}


void Projectiles::Render()
{
	//Camera init
	glm::vec2 offset = glm::vec2(float(cSettings->NUM_TILES_XAXIS / 2.f), float(cSettings->NUM_TILES_YAXIS / 2.f));
	glm::vec2 cameraPos = Camera2D::GetInstance()->getCurrPos();

	glm::vec2 objCamPos = vTransform - cameraPos + offset;

	glm::vec2 actualPos = cSettings->ConvertIndexToUVSpace(objCamPos) * Camera2D::GetInstance()->getZoom();

	float clampOffset = cSettings->ConvertIndexToUVSpace(CSettings::AXIS::x, 1, false) / 2;

	float clampX = 2.0f + clampOffset;
	float clampY = 2.0f + clampOffset;

	if (actualPos.x >= -clampX && actualPos.x <= clampX && actualPos.y >= -clampY && actualPos.y <= clampY)
	{
		transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		transform = glm::translate(transform, glm::vec3(glm::vec2(actualPos.x, actualPos.y),
			0.0f));
		transform = glm::rotate(transform, glm::radians(fRotate), glm::vec3(0.f, 0.f, 1.f));
		transform = glm::scale(transform, glm::vec3(Camera2D::GetInstance()->getZoom()));

		CShaderManager::GetInstance()->activeShader->setMat4("transform", transform);
		CShaderManager::GetInstance()->activeShader->setVec4("runtime_color", currentColor);

		glBindTexture(GL_TEXTURE_2D, CTextureManager::GetInstance()->MapOfTextureIDs.at(iTextureID));
		glBindVertexArray(VAO);
		mesh->Render();
		glBindVertexArray(0);
	}
}

void Projectiles::PreRender()
{
	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use("2DShader");
}

void Projectiles::PostRender()
{
	// Disable blending
	glDisable(GL_BLEND);
}

bool Projectiles::bOutsideBoundary(void)
{
	glm::vec2 limit = CMap2D::GetInstance()->GetLevelLimit();

	bool xCheck = (vTransform.x < -collider2D->vec2Dimensions.x || vTransform.x > limit.x + collider2D->vec2Dimensions.x);
	bool yCheck = (vTransform.y < -collider2D->vec2Dimensions.y || vTransform.y > limit.y + collider2D->vec2Dimensions.y);

	return (xCheck || yCheck);
}

void Projectiles::ResolveEnemyCollision()
{
	std::vector<CEnemy2D*> enemyArr = cEntityManager->GetAllEnemies();

	//collider2D->CollideWith(obj->GetCollider())
	for (unsigned i = 0; i < enemyArr.size(); i++) {
		Collision data = collider2D->CollideWith(enemyArr[i]->GetCollider());

		if (std::get<0>(data)) { //If collided with 
			//Enemy collision code activate!
			CMobEnemy2D* mobEnemy = dynamic_cast<CMobEnemy2D*>(enemyArr[i]);

			if (mobEnemy)
			{
				mobEnemy->Attacked(1, cPhysics2D);
			}
			else
			{
				enemyArr[i]->Attacked();
			}

			bDestroyed = true;
		}
	}
}

