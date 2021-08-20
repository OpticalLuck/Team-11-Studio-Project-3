#include "MobEnemy2D.h"

//Include handlers
#include "Map2D.h"
#include "Camera2D.h"
#include "EntityManager.h"

//Mesh builder
#include "Primitives/MeshBuilder.h"

//Manager
#include "RenderControl/ShaderManager.h"

CMobEnemy2D::CMobEnemy2D(void) {
	animatedSprites = nullptr;
}

CMobEnemy2D::~CMobEnemy2D(void) {
	// de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

bool CMobEnemy2D::Init(void) {
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();

	camera = Camera2D::GetInstance();

	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(300, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	vTransform = glm::i32vec2(uiCol, uiRow);

	roundIndex = 0;
	health = 5;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS: Create the Quad Mesh using the mesh builder
	quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	// Load the enemy2D texture
	if (LoadTexture("Image/Enemy/Mushroom.png", iTextureID) == false)
	{
		std::cout << "Failed to load mobenemy2D tile texture" << std::endl;
		return false;
	}

	//Store initial value of each round
	for (int i = 0; i < 5; i++) {
		roundDir[i] = RandomiseDir();
	}
	dir = roundDir[roundIndex];

	//CS: Init the color to white
	currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

	//Get handlers to all player objects
	currTarget = nullptr;
	arrPlayer = CEntityManager::GetInstance()->GetAllPlayers();

	cEntityManager = CEntityManager::GetInstance();

	// Set the Physics to fall status by default
	//cPhysics2D.Init();
	//cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

	//Collider2D
	collider2D->Init();
	//Update collider to Boss position
	collider2D->SetPosition(glm::vec3(vTransform, 0.f));

	//Animation
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(4, 3, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("move", 0, 2);

	animatedSprites->PlayAnimation("move", -1, 0.6f);

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;

	return true;
}

void CMobEnemy2D::Update(const double dElapsedTime) {
	//Collision and stuff (Refer to Jeryl's new branch)

	animatedSprites->Update(dElapsedTime);
}

void CMobEnemy2D::Render(void) {
	if (!bIsActive)
		return;

	glBindVertexArray(VAO);
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	unsigned int colorLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "runtime_color");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	//Get camera transforms and use them instead
	glm::vec2 offset = glm::vec2((cSettings->NUM_TILES_XAXIS / 2.f) , (cSettings->NUM_TILES_YAXIS / 2.f));
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

	if (dir == DIRECTION::LEFT) //Rotate if facing other way
		transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));

		// Update the shaders with the latest transform
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform4fv(colorLoc, 1, glm::value_ptr(currentColor));

	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);

	// Render the tile
	animatedSprites->Render();

	glBindVertexArray(0);
}
