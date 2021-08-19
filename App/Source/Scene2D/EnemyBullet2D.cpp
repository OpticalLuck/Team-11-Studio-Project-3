#include "EnemyBullet2D.h"

//CMeshbuilder
#include "../Library/Source/Primitives/MeshBuilder.h"

//Include shader
#include "../Library/Source/RenderControl/ShaderManager.h"

EnemyBullet2D::EnemyBullet2D(void) {
	dirVec = glm::vec2();
	mSpd= mScale = mAngle = 0;
}

EnemyBullet2D::~EnemyBullet2D(void) {
	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

bool EnemyBullet2D::Init(float angle, glm::vec2 spawnPoint) {
	//Handlers
	cEntityManager = CEntityManager::GetInstance();
	cMap2D = CMap2D::GetInstance();
	cSettings = CSettings::GetInstance();
	camera = Camera2D::GetInstance();

	//Convert angle to vector
	
	//Convert to radian first
	mAngle = angle;
	angle = Math::DegreeToRadian(angle);
	vTransform = spawnPoint;

	dirVec = glm::vec2(cosf(angle), sinf(angle));
	mSpd = 10; //index per second
	mScale = 0.5f;

	health = 1;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS: Create the Quad Mesh using the mesh builder
	if (!quadMesh)
		quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH * mScale, cSettings->TILE_HEIGHT * mScale);

	// Load the enemy2D texture
	if (LoadTexture("Image/Objects/Bullet.tga", iTextureID) == false)
	{
		std::cout << "Failed to load bullet tile texture" << std::endl;
		return false;
	}

	//CS: Init the color to white
	currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;

	collider2D->vec2Dimensions = glm::vec2(0.25f, 0.25f);
	collider2D->Init();
	//Update collider to position
	collider2D->SetPosition(glm::vec3(vTransform, 0.f));
	collider2D->SetAngle(mAngle);
}

void EnemyBullet2D::Update(const double dElapsedTime) {
	//Move the bullet
	vTransform += dirVec * mSpd * (float)dElapsedTime;

	//Update collider to position
	collider2D->SetPosition(glm::vec3(vTransform, 0.f));
	collider2D->SetAngle(mAngle);

	//Collision code here...
}

void EnemyBullet2D::Render(void) {
	if (!bIsActive)
		return;

	glBindVertexArray(VAO);
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	unsigned int colorLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "runtime_color");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	//Get camera transforms and use them instead
	glm::vec2 offset = glm::vec2((cSettings->NUM_TILES_XAXIS / 2.f) - 0.5f, (cSettings->NUM_TILES_YAXIS / 2.f) - 0.5f);
	glm::vec2 cameraPos = camera->getCurrPos();

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
	glUniform4fv(colorLoc, 1, glm::value_ptr(currentColor));

	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);

	// Render the tile
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	quadMesh->Render();

	glBindVertexArray(0);
}

bool EnemyBullet2D::OutOfWorld(void) {
	glm::vec2 limit = cMap2D->GetLevelLimit();

	bool xCheck = (vTransform.x < -mScale / 2 || vTransform.x > limit.x + mScale / 2);
	bool yCheck = (vTransform.y < -mScale / 2 || vTransform.y > limit.y + mScale / 2);

	return (xCheck || yCheck);
}
