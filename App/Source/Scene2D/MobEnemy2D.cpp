#include "MobEnemy2D.h"

//Include handlers
#include "Map2D.h"
#include "Camera2D.h"
#include "EntityManager.h"

//Mesh builder
#include "Primitives/MeshBuilder.h"

//Manager
#include "RenderControl/ShaderManager.h"

//Interactables
#include "Interactables.h"
#include "Boulder2D.h"

CMobEnemy2D::CMobEnemy2D(void) {
	animatedSprites = nullptr;
	mSpd = 0;
	oldVTransform = glm::vec2();
	clampSides = true;
	inView = false;
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

	int id = 300;
	if (!clampSides)
		id = 301;

	if (cMap2D->FindValue(id, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0, CLASS_ID::CID_NONE);

	// Set the start position of the Player to iRow and iCol
	vTransform = glm::i32vec2(uiCol, uiRow);
	oldVTransform = vTransform;

	roundIndex = 0;
	health = 5;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS: Create the Quad Mesh using the mesh builder
	quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

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

	//Collider2D
	collider2D->Init(vTransform);

	//Animation
	/*animatedSprites = CMeshBuilder::GenerateSpriteAnimation(4, 3, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("idle", 0, 2);

	animatedSprites->PlayAnimation("idle", -1, 0.6f);*/

	//Physics initialisation
	cPhysics2D.Init(&vTransform);
	mSpd = 5;

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;
	inView = false;

	return true;
}

void CMobEnemy2D::SetAnimatedSprites(CSpriteAnimation* animatedSprites) {
	this->animatedSprites = animatedSprites;
}

CSpriteAnimation* CMobEnemy2D::GetAnimatedSprites(void) {
	return animatedSprites;
}

void CMobEnemy2D::SetClampSlides(bool clamp) {
	clampSides = clamp;
}

void CMobEnemy2D::Update(const double dElapsedTime) {
	if (!inView) { //Do not activate if enemy is out of view
		if (WithinProjectedCamera(cEntityManager->GetPlayer()))
			inView = true;

		return;
	}

	// Store the old position
	oldVTransform = vTransform;

	//Clamping of position
	ClampPos();

	//Physics
	UpdateMovement();
	cPhysics2D.Update(dElapsedTime);

	//Collision with world's object
	collider2D->position = vTransform;
	CollisionUpdate();

	if (vTransform == oldVTransform) {
		if (dir == DIRECTION::LEFT)
			dir = DIRECTION::RIGHT;
		else
			dir = DIRECTION::LEFT;
	}

	animatedSprites->Update(dElapsedTime);
}

void CMobEnemy2D::ClampPos(void) {
	if (!clampSides)
		return; //Do not clamp sides if option is not set

	glm::vec2 tileCheck = glm::vec2(round(vTransform.x), round(vTransform.y));
	tileCheck.y--; //Going one tile down to check object below player

	float xCheck = vTransform.x;
	xCheck = xCheck - floorf(xCheck);

	float dist = 0.1f; //Checks how near player should be at the edge before its activated

	if (1 - xCheck < xCheck)
		xCheck = 1 - xCheck;

	if (xCheck > dist)
		return; //If its not near the edge, dont activate yet

	switch (dir) {
		case DIRECTION::LEFT: {
			tileCheck.x--;
			CObject2D* obj = cMap2D->GetCObject(tileCheck.x, tileCheck.y);

			if (!obj) {
				dir = DIRECTION::RIGHT;
				vTransform = glm::vec2(round(vTransform.x), round(vTransform.y));
			}
			break;
		}

		case DIRECTION::RIGHT: {
			tileCheck.x++;
			CObject2D* obj = cMap2D->GetCObject(tileCheck.x, tileCheck.y);

			if (!obj) {
				dir = DIRECTION::LEFT;
				vTransform = glm::vec2(round(vTransform.x), round(vTransform.y));
			}

			break;
		}

		default:
			DEBUG_MSG("ERROR IN CLAMPING, CMOBENEMY2D DIRECTION SEEMS TO BE INVALID. PLEASE FIX!");
			break;
	}
}

void CMobEnemy2D::CollisionUpdate(void) {
	int range = 3;
	cPhysics2D.SetboolGrounded(false);

	//Collision for loop
	for (int i = 0; i < 2; i++)
	{
		for (int row = -range; row <= range; row++) //y
		{
			for (int col = -range; col <= range; col++) //x
			{
				int rowCheck = vTransform.y + row;
				int colCheck = vTransform.x + col;

				if (rowCheck < 0 || colCheck < 0 || rowCheck > cMap2D->GetLevelRow() - 1 || colCheck > cMap2D->GetLevelCol() - 1) continue;

				if (cMap2D->GetCObject(colCheck, rowCheck))
				{
					CObject2D* obj = cMap2D->GetCObject(colCheck, rowCheck);;
					Collision data = (collider2D->CollideWith(cMap2D->GetCObject(colCheck, rowCheck)->GetCollider()));
					if (std::get<0>(data))
					{
						if (obj->GetCollider()->colliderType == Collider2D::COLLIDER_QUAD)
						{
							if (i == 0)
							{
								collider2D->ResolveAABB(obj->GetCollider(), Direction::UP);
							}
							else if (i == 1)
							{
								collider2D->ResolveAABB(obj->GetCollider(), Direction::RIGHT);
							}

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

						//Clamping movement...
						vTransform = collider2D->position;
						obj->vTransform = obj->GetCollider()->position;

						if (obj->type == ENTITY_TYPE::TILE)
						{
							if (dynamic_cast<Boulder2D*>(obj))
							{
								glm::vec2 direction = glm::normalize(obj->vTransform - vTransform);
								static_cast<Boulder2D*>(obj)->GetPhysics().SetForce(glm::vec2(120.f, 0) * direction);
								cPhysics2D.SetVelocity(glm::vec2(0.f));
							}
						}
					}
				}
			}
		}
	}

	//Player collision
	std::vector<CPlayer2D*> arrPlayer = cEntityManager->GetAllPlayers();

	for (unsigned i = 0; i < arrPlayer.size(); i++) {
		Collider2D* playerCollider = arrPlayer[i]->GetCollider();
		Collision data = (collider2D->CollideWith(playerCollider));

		if (std::get<0>(data)) {
			//What happens when player collides with enemy code below
			arrPlayer[i]->Attacked();
			
			return;
		}
	}
}

void CMobEnemy2D::UpdateMovement(void) {
	float force = mSpd;

	if (dir == DIRECTION::LEFT)
		force *= -1;

	glm::vec2 velocity = cPhysics2D.GetVelocity();
	velocity.x = force;

	cPhysics2D.SetVelocity(velocity);
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
