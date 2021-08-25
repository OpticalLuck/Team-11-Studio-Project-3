#include "Bullet2D.h"

#include "Map2D.h"
#include <vector>
#include "RenderControl/ShaderManager.h"
#include "Primitives/MeshBuilder.h"
#include "Math/MyMath.h"
#include "EntityManager.h"
#include "MobEnemy2D.h"

Bullet2D::Bullet2D(int iTextureID)
{
	this->iTextureID = iTextureID; //Shuriken also by default
	mFriendly = false;
}

Bullet2D::~Bullet2D()
{
	if (animatedSprites)
		delete animatedSprites;

	glDeleteVertexArrays(1, &VAO);
}

bool Bullet2D::Init(bool player, float angle, float force)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	fRotate = angle;

	cSettings = CSettings::GetInstance();
	mesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	if (!cPhysics2D)
		cPhysics2D = new CPhysics2D;
	if (!collider2D)
		collider2D = new Collider2D;

	cPhysics2D->Init(&vTransform);
	cPhysics2D->SetGravity(0.f);
	cPhysics2D->MAX_SPEED = 50.f;

	float radAng = Math::DegreeToRadian(angle);
	cPhysics2D->SetVelocity(glm::vec2(cosf(radAng), sinf(radAng)) * force);
	cPhysics2D->SetMass(0.75f);

	collider2D->Init(vTransform, glm::vec2(0.25f), Collider2D::ColliderType::COLLIDER_CIRCLE);

	//Handlers
	cEntityManager = CEntityManager::GetInstance();
	return true;
}

void Bullet2D::Update(double dElapsedTime)
{
	cPhysics2D->Update(dElapsedTime);
	collider2D->SetPosition(vTransform);

	MapCollision();
	if (!bDestroyed)
		EntityCollision();
}

void Bullet2D::EntityCollision(void) {
	std::vector<LivingEntity2D*> arr;
	if (mFriendly) {
		std::vector<CEnemy2D*> ogArr = cEntityManager->GetAllEnemies();
		arr = std::vector<LivingEntity2D*>(ogArr.begin(), ogArr.end());
	}
	else {
		std::vector<CPlayer2D*> ogArr = cEntityManager->GetAllPlayers();
		arr = std::vector<LivingEntity2D*>(ogArr.begin(), ogArr.end());
	}

	for (unsigned i = 0; i < arr.size(); i++) {
		Collider2D* playerCollider = dynamic_cast<CEntity2D*>(arr[i])->GetCollider();
		Collision data = (collider2D->CollideWith(playerCollider));

		if (std::get<0>(data)) {
			//collision code below
			CPlayer2D* player = dynamic_cast<CPlayer2D*>(arr[i]);
			CMobEnemy2D* enemy = dynamic_cast<CMobEnemy2D*>(arr[i]);

			//For knockback effect
			if (player)
				player->Attacked(1, cPhysics2D);
			else if (enemy)
				enemy->Attacked(1, cPhysics2D);
			else
				arr[i]->Attacked(); //Default

			//Remove bullet from worldspace
			bDestroyed = true;
			return;
		}
	}
}

void Bullet2D::MapCollision(void) {
	CMap2D* cMap2D = CMap2D::GetInstance();

	ResolveMapCollision(CheckMapCollision());
}

void Bullet2D::PreRender()
{
	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	CShaderManager::GetInstance()->Use("2DShader");
	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	// Activate the shader
}

void Bullet2D::Render()
{
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	//Camera init
	glm::vec2 offset = glm::vec2(float(cSettings->NUM_TILES_XAXIS / 2.f), float(cSettings->NUM_TILES_YAXIS / 2.f));
	glm::vec2 cameraPos = Camera2D::GetInstance()->getCurrPos();

	glm::vec2 objCamPos = vTransform - cameraPos + offset;

	glm::vec2 actualPos = cSettings->ConvertIndexToUVSpace(objCamPos);

	float clampOffset = cSettings->ConvertIndexToUVSpace(CSettings::AXIS::x, 1, false) / 2;

	float clampX = 2.0f + clampOffset;
	float clampY = 2.0f + clampOffset;

	if (actualPos.x >= -clampX && actualPos.x <= clampX && actualPos.y >= -clampY && actualPos.y <= clampY)
	{
		transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		transform = glm::translate(transform, glm::vec3(glm::vec2(actualPos.x, actualPos.y),
			0.0f));
		transform = glm::rotate(transform, glm::radians(fRotate), glm::vec3(0.f, 0.f, 1.f));
		transform = glm::scale(transform, glm::vec3(collider2D->vec2Dimensions.x*2, collider2D->vec2Dimensions.y*2,1));
		// Update the shaders with the latest transform
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		// Get the texture to be rendered
		glBindTexture(GL_TEXTURE_2D, CTextureManager::GetInstance()->MapOfTextureIDs.at(iTextureID));

		glBindVertexArray(VAO);
		mesh->Render();

		glBindVertexArray(0);
	}
}

void Bullet2D::PostRender()
{
	// Disable blending
	glDisable(GL_BLEND);
}

void Bullet2D::ResolveMapCollision(std::vector<pair<CObject2D*, float>> aabbvector)
{
	for (auto aabbTuple : aabbvector)
	{
		CObject2D* obj = aabbTuple.first;
		Collision data = (collider2D->CollideWith(obj->GetCollider()));
		if (std::get<0>(data))
		{
			bDestroyed = true;
		}
	}
}

