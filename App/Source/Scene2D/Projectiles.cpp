#include "Projectiles.h"

#include "Map2D.h"
#include <vector>

#include "EntityManager.h"
#include "Primitives/MeshBuilder.h"
#include "RenderControl/ShaderManager.h"
#include "../TextureManager/TextureManager.h"
#include "Camera2D.h"
Projectiles::Projectiles(int iTextureID)
	: animatedSprites(NULL)
	, currentColor(glm::vec4())
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

	cPhysics2D.Init(&vTransform);
	cPhysics2D.MAX_SPEED = 50.f;
	cPhysics2D.FRICTONAL_COEFFICIENT = 0.8f;

	collider2D->Init(vTransform, glm::vec2(0.2f), Collider2D::ColliderType::COLLIDER_CIRCLE);

	return false;
}

void Projectiles::Update(double dElapsedTime)
{
	cPhysics2D.Update(dElapsedTime);
	// Update Collider2D Position
	collider2D->SetPosition(vTransform);

	//Collision between objects in map space
	MapCollision();
}

void Projectiles::MapCollision(void) {
	CMap2D* cMap2D = CMap2D::GetInstance();

	int range = 2;
	cPhysics2D.SetboolGrounded(false);
	vector<pair<CObject2D*, float>> aabbVector;
	for (int row = -range; row <= range; row++) //y
	{
		for (int col = -range; col <= range; col++) //x
		{
			int rowCheck = vTransform.y + row;
			int colCheck = vTransform.x + col;

			if (rowCheck < 0 || colCheck < 0 || rowCheck > cMap2D->GetLevelRow() - 1 || colCheck > cMap2D->GetLevelCol() - 1) continue;

			if (cMap2D->GetCObject(colCheck, rowCheck) && cMap2D->GetCObject(colCheck, rowCheck) != this)
			{
				CObject2D* obj = cMap2D->GetCObject(colCheck, rowCheck);
				float distance = glm::length(obj->vTransform - vTransform);
				aabbVector.push_back({ obj, distance });
			}
		}
	}
	//Sorts vector based on shortest dist from player to object
	sort(aabbVector.begin(), aabbVector.end(), [](const std::pair<CObject2D*, float>& a, const std::pair<CObject2D*, float>& b)
		{
			return a.second < b.second;
		});
	aabbVector.erase(std::unique(aabbVector.begin(), aabbVector.end()), aabbVector.end());
	bool destroyed = false;
	for (auto aabbTuple : aabbVector)
	{
		CObject2D* obj = aabbTuple.first;
		Collision data = (collider2D->CollideWith(obj->GetCollider()));
		if (std::get<0>(data))
		{
			if (obj->GetCollider()->colliderType == Collider2D::COLLIDER_QUAD)
			{
				collider2D->ResolveAABB(obj->GetCollider(), Direction::UP);
				collider2D->ResolveAABB(obj->GetCollider(), Direction::RIGHT);

				if (std::get<1>(data) == Direction::UP)
					cPhysics2D.SetboolGrounded(true);
			}
			else if (obj->GetCollider()->colliderType == Collider2D::COLLIDER_CIRCLE)
			{
				if (glm::dot(cPhysics2D.GetVelocity(), obj->vTransform - vTransform) > 0)
					collider2D->ResolveAABBCircle(obj->GetCollider(), data, Collider2D::COLLIDER_QUAD);

				if (std::get<1>(data) == Direction::DOWN)
					cPhysics2D.SetboolGrounded(true);
			}

			vTransform = collider2D->position;

			//How the object interacts 
			glm::vec2 normal = Collider2D::ConvertDirectionToVec2(std::get<1>(data));
			cPhysics2D.DoBounce(normal, 0.f);
		}
	}
}

void Projectiles::PlayerCollision(void) {
	std::vector<CPlayer2D*> playerArr = cEntityManager->GetAllPlayers();

	//collider2D->CollideWith(obj->GetCollider())
	for (unsigned i = 0; i < playerArr.size(); i++) {
		//Collision data = 
	}
}

CPhysics2D& Projectiles::GetPhysics()
{
	return cPhysics2D;
}

void Projectiles::Render()
{
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
		// get matrix's uniform location and set matrix
		unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		transform = glm::translate(transform, glm::vec3(glm::vec2(actualPos.x, actualPos.y),
			0.0f));
		transform = glm::rotate(transform, glm::radians(fRotate), glm::vec3(0.f, 0.f, 1.f));
		// Update the shaders with the latest transform
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		// Get the texture to be rendered
		glBindTexture(GL_TEXTURE_2D, CTextureManager::GetInstance()->MapOfTextureIDs.at(iTextureID));

		glBindVertexArray(VAO);

		//CS: Use mesh to render
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
