#include "Obstacle2D.h"
#include "Map2D.h"

#include "RenderControl/ShaderManager.h"
#include "../TextureManager/TextureManager.h"
#include "Primitives/Camera2D.h"
#include "Primitives/MeshBuilder.h"
#include "EntityManager.h"
#include "MobEnemy2D.h"

Obstacle2D::Obstacle2D(int iTextureID)
{
	this->iTextureID = iTextureID;
	type = ENTITY_TYPE::TILE;
	force = 1500;
}

Obstacle2D::~Obstacle2D()
{
}

bool Obstacle2D::Init()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	mesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	if (!cPhysics2D)
		cPhysics2D = new CPhysics2D;
	if (!collider2D)
		collider2D = new Collider2D;

	cEntityManager = CEntityManager::GetInstance();

	cPhysics2D->Init(&vTransform);
	cPhysics2D->SetMass(10);
	collider2D->Init(vTransform, glm::vec2(0.5f), Collider2D::ColliderType::COLLIDER_CIRCLE);
	return true;
}

void Obstacle2D::ResolveEnemyCollision(void) {
	//Enemy collision
	std::vector<CEnemy2D*> arrEnemy = CEntityManager::GetInstance()->GetAllEnemies();

	for (unsigned i = 0; i < arrEnemy.size(); i++) {
		CMobEnemy2D* cEnemy = dynamic_cast<CMobEnemy2D*>(arrEnemy[i]);
		if (!cEnemy)
			continue;

		Collider2D* enemyCollider = arrEnemy[i]->GetCollider();
		Collision data = (enemyCollider->CollideWith(collider2D));
		if (std::get<0>(data) ) {
			//Player collision code below
			enemyCollider->ResolveAABBCircle(collider2D, data, Collider2D::ColliderType::COLLIDER_QUAD);

			glm::vec2 newVel = cEnemy->GetPhysics()->GetVelocity();
			newVel.x = 0;
			cEnemy->GetPhysics()->SetVelocity(newVel);

			cEnemy->vTransform = cEnemy->GetCollider()->position;
			vTransform = collider2D->position;

			if (std::get<1>(data) == Direction::UP)
				cEnemy->GetPhysics()->SetboolGrounded(true);
		}
	}
}

void Obstacle2D::Update(const double dElapsedTime)
{
	cPhysics2D->Update(dElapsedTime);
	// Update Collider2D Position
	collider2D->position = vTransform;

	CMap2D* cMap2D = CMap2D::GetInstance();

	cPhysics2D->SetboolGrounded(false);
	ResolvePlayerCollision();
	ResolveEnemyCollision();
	ResolveMapCollision(CheckMapCollision());
	ResolveObstacleCollision();
}

void Obstacle2D::PreRender()
{
	
// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use("2DShader");
}

void Obstacle2D::Render()
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

		glBindTexture(GL_TEXTURE_2D, CTextureManager::GetInstance()->MapOfTextureIDs.at(iTextureID));

		glBindVertexArray(VAO);
		mesh->Render();
		glBindVertexArray(0);
	}
}

void Obstacle2D::PostRender()
{
	// Disable blending
	glDisable(GL_BLEND);
}

void Obstacle2D::ResolveObstacleCollision(void) {
	std::vector<Obstacle2D*> arrObj = cEntityManager->GetallObstacles();

	for (unsigned i = 0; i < arrObj.size(); ++i) {
		float maxDist = 1.f;
		float dist = glm::length(arrObj[i]->vTransform - vTransform);

		if (dist > maxDist)
			continue;

		CObject2D* obj = arrObj[i];
		Collision data = collider2D->CollideWith(arrObj[i]->GetCollider());
		if (std::get<0>(data)) {
			if (obj->GetCollider()->colliderType == Collider2D::ColliderType::COLLIDER_QUAD)
			{
				if (collider2D->colliderType == Collider2D::ColliderType::COLLIDER_CIRCLE)
				{
					obj->GetCollider()->ResolveAABBCircle(collider2D, data, Collider2D::ColliderType::COLLIDER_CIRCLE);

					if (std::get<1>(data) == Direction::DOWN)
						cPhysics2D->SetboolGrounded(true);
				}
				else
				{
					collider2D->ResolveAABB(obj->GetCollider(), data);

					if (std::get<1>(data) == Direction::UP)//kinda reversed xd
						cPhysics2D->SetboolGrounded(true);
				}
			}
			else if (obj->GetCollider()->colliderType == Collider2D::ColliderType::COLLIDER_CIRCLE)
			{
				if (glm::dot(cPhysics2D->GetVelocity(), obj->vTransform - vTransform) > 0)
					collider2D->ResolveAABBCircle(obj->GetCollider(), data, Collider2D::ColliderType::COLLIDER_QUAD);

				if (std::get<1>(data) == Direction::DOWN)
					cPhysics2D->SetboolGrounded(true);
			}

			vTransform = collider2D->position;
			obj->vTransform = obj->GetCollider()->position;

			if (std::get<1>(data) == Direction::LEFT || std::get<1>(data) == Direction::RIGHT)
				cPhysics2D->SetVelocity(glm::vec2(0, cPhysics2D->GetVelocity().y));
		}
	}
}

void Obstacle2D::ResolveMapCollision(std::vector<pair<CObject2D*, float>> aabbvector)
{
	for (auto aabbTuple : aabbvector)
	{
		CObject2D* obj = aabbTuple.first;
		Collision data = (collider2D->CollideWith(obj->GetCollider()));
		if (std::get<0>(data))
		{
			if (obj->GetCollider()->colliderType == Collider2D::ColliderType::COLLIDER_QUAD)
			{
				if (collider2D->colliderType == Collider2D::ColliderType::COLLIDER_CIRCLE)
				{
					obj->GetCollider()->ResolveAABBCircle(collider2D, data, Collider2D::ColliderType::COLLIDER_CIRCLE);

					if (std::get<1>(data) == Direction::DOWN)
						cPhysics2D->SetboolGrounded(true);
				}
				else
				{
					collider2D->ResolveAABB(obj->GetCollider(), data);

					if (std::get<1>(data) == Direction::UP)//kinda reversed xd
						cPhysics2D->SetboolGrounded(true);
				}
			}
			else if (obj->GetCollider()->colliderType == Collider2D::ColliderType::COLLIDER_CIRCLE)
			{
				if (glm::dot(cPhysics2D->GetVelocity(), obj->vTransform - vTransform) > 0)
					collider2D->ResolveAABBCircle(obj->GetCollider(), data, Collider2D::ColliderType::COLLIDER_QUAD);

				if (std::get<1>(data) == Direction::DOWN)
					cPhysics2D->SetboolGrounded(true);
			}

			vTransform = collider2D->position;
			obj->vTransform = obj->GetCollider()->position;

			if (std::get<1>(data) == Direction::LEFT || std::get<1>(data) == Direction::RIGHT)
				cPhysics2D->SetVelocity(glm::vec2(0, cPhysics2D->GetVelocity().y));
		}
	}
}

void Obstacle2D::ResolvePlayerCollision()
{
	//Player collision
	std::vector<CPlayer2D*> arrPlayer = CEntityManager::GetInstance()->GetAllPlayers();

	for (unsigned i = 0; i < arrPlayer.size(); i++) {
		CPlayer2D* cPlayer = arrPlayer[i];
		Collider2D* playerCollider = arrPlayer[i]->GetCollider();
		Collision data = (playerCollider->CollideWith(collider2D));
		if (std::get<0>(data)) {
			//Player collision code below
			playerCollider->ResolveAABBCircle(collider2D, data, Collider2D::ColliderType::COLLIDER_QUAD);

			glm::vec2 direction = glm::normalize(vTransform - cPlayer->vTransform);
			cPhysics2D->SetForce(glm::vec2(force, 0) * direction);

			//cPlayer->GetCPhysics()->SetVelocity(glm::vec2(0.f));

			cPlayer->vTransform = cPlayer->GetCollider()->position;
			vTransform = collider2D->position;


			glm::vec2 vel = cPlayer->GetCPhysics()->GetVelocity();
			vel.x = 0;

			if (std::get<1>(data) == Direction::UP) {
				cPlayer->GetPhysics()->SetboolGrounded(true);
				cPlayer->m_playerState = CPlayer2D::STATE::S_IDLE;
				cPlayer->SetJumpCount(0);

				if (vel.y != 0) 
					cPlayer->GetPhysics()->SetVelocity(vel);
				else 
					cPlayer->GetCPhysics()->SetVelocity(glm::vec2(0.f));
			}
			else {
				cPlayer->GetCPhysics()->SetVelocity(glm::vec2(0.f));
				//cout << "grounded" << endl;
			}
		}
	}
}
