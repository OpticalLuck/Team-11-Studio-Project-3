#include "EnemyBullet2D.h"

//CMeshbuilder
#include "../Library/Source/Primitives/MeshBuilder.h"

//Include shader
#include "../Library/Source/RenderControl/ShaderManager.h"

//Map
#include "Map2D.h"

EnemyBullet2D::EnemyBullet2D(void) {
	dirVec = glm::vec2();
	mForce= mScale = mAngle = 0;
	cMap2D = nullptr;
	pHealth = 0;
}

EnemyBullet2D::~EnemyBullet2D(void) {
	cMap2D = nullptr;

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

bool EnemyBullet2D::Init(float angle, glm::vec2 spawnPoint) {
	//Handlers
	cEntityManager = CEntityManager::GetInstance();
	cSettings = CSettings::GetInstance();
	cMap2D = CMap2D::GetInstance();
	camera2D = Camera2D::GetInstance();
	cMap2D = CMap2D::GetInstance();

	//Convert angle to vector
	
	//Convert to radian first
	mAngle = angle;
	angle = Math::DegreeToRadian(angle);
	vTransform = spawnPoint;

	dirVec = glm::vec2(cosf(angle), sinf(angle));
	mForce = 10; //index per second
	mScale = 0.5f;

	pHealth = 1;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS: Create the Quad Mesh using the mesh builder
	if (!mesh)
		mesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH * mScale, cSettings->TILE_HEIGHT * mScale);

	// Load the enemy2D texture
	if (LoadTexture("Image/Objects/Bullet.tga") == false)
	{
		std::cout << "Failed to load bullet tile texture" << std::endl;
		return false;
	}

	if (!cPhysics2D)
		cPhysics2D = new CPhysics2D;
	if (!collider2D)
		collider2D = new Collider2D;

	collider2D->Init(vTransform, glm::vec2(mScale/2), Collider2D::ColliderType::COLLIDER_CIRCLE);
	//Update collider to position
	collider2D->SetPosition(glm::vec3(vTransform, 0.f));

	return true;
}

void EnemyBullet2D::Update(const double dElapsedTime) {
	//Move the bullet
	vTransform += dirVec * mForce * (float)dElapsedTime;

	//Update collider to position
	collider2D->SetPosition(glm::vec3(vTransform, 0.f));

	//Collision code here...
	CollisionCheck();

	vTransform = collider2D->GetPosition();
}

void EnemyBullet2D::CollisionCheck(void) {
	ResolveMapCollision(CheckMapCollision());
	ResolvePlayerCollision();
}

void EnemyBullet2D::Render(void) {
	glBindVertexArray(VAO);
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	unsigned int colorLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "runtime_color");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	//Get camera transforms and use them instead
	glm::vec2 offset = glm::vec2((cSettings->NUM_TILES_XAXIS / 2.f), (cSettings->NUM_TILES_YAXIS / 2.f));
	glm::vec2 cameraPos = camera2D->getCurrPos();

	glm::vec2 IndexPos = vTransform;

	glm::vec2 actualPos = IndexPos - cameraPos + offset;
	actualPos = cSettings->ConvertIndexToUVSpace(actualPos);

	float clampOffset = cSettings->ConvertIndexToUVSpace(CSettings::AXIS::x, 1, false);
	clampOffset = (clampOffset + 1);

	float clampX = 1.0f + clampOffset;
	float clampY = 1.0f + clampOffset;
	if (actualPos.x <= -clampX || actualPos.x >= clampX || actualPos.y <= -clampY || actualPos.y >= clampY)
		return; //Exit code if enemy is too far to be rendered

	transform = glm::translate(transform, glm::vec3(actualPos.x, actualPos.y, 0.f));
	transform = glm::rotate(transform, glm::radians(mAngle), glm::vec3(0.f, 0.f, 1.f));

	// Update the shaders with the latest transform
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	//glUniform4fv(colorLoc, 1, glm::value_ptr(currentColor));

	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);

	// Render the tile
	mesh->Render();

	glBindVertexArray(0);
}

bool EnemyBullet2D::OutOfWorld(void) {
	glm::vec2 limit = cMap2D->GetLevelLimit();

	bool xCheck = (vTransform.x < -mScale / 2 || vTransform.x > limit.x + mScale / 2);
	bool yCheck = (vTransform.y < -mScale / 2 || vTransform.y > limit.y + mScale / 2);

	return (xCheck || yCheck);
}

int EnemyBullet2D::GetHealth(void) {
	return pHealth;
}

void EnemyBullet2D::ResolveMapCollision(std::vector<pair<CObject2D*, float>> aabbvector)
{
	for (auto aabbTuple : aabbvector)
	{
		CObject2D* obj = aabbTuple.first;
		Collision data = (collider2D->CollideWith(obj->GetCollider()));
		if (std::get<0>(data))
		{
			pHealth = 0; //If health = 0, bullet will no longer be rendered
			return;
		}
	}
}

void EnemyBullet2D::ResolvePlayerCollision()
{
	//Player collision
	std::vector<CPlayer2D*> arrPlayer = cEntityManager->GetAllPlayers();

	for (unsigned i = 0; i < arrPlayer.size(); i++) {
		Collider2D* playerCollider = arrPlayer[i]->GetCollider();
		Collision data = (collider2D->CollideWith(playerCollider));

		if (std::get<0>(data)) {
			//Player collision code below
			arrPlayer[i]->Attacked();

			//Remove bullet from worldspace
			pHealth = 0;
			return;
		}
	}
}
