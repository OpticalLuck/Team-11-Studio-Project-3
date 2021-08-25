#include "MobEnemy2D.h"

//Include handlers
#include "Map2D.h"
#include "Primitives/Camera2D.h"
#include "EntityManager.h"

//Mesh builder
#include "Primitives/MeshBuilder.h"

//Manager
#include "RenderControl/ShaderManager.h"

//Interactables
#include "Interactables.h"
#include "Boulder2D.h"

#include "RayCast2D.h"

CMobEnemy2D::CMobEnemy2D(void) {
	animatedSprites = nullptr;
	mSpd = 0;
	oldVTransform = glm::vec2();
	clampSides = true;
	inView = false;
	rayCast2D = nullptr;
	patrol = false;

	intervalTimer = stateTimer = 0;

	maxStateTimer[0] = 0;
	maxStateTimer[1] = 0;
	maxStateTimer[2] = 0;
	maxStateTimer[3] = 0;

	maxIntervalTimer[0] = 0;
	maxIntervalTimer[1] = 0;
	maxIntervalTimer[2] = 0;
	maxIntervalTimer[3] = 0;
}

CMobEnemy2D::~CMobEnemy2D(void) {
	if (rayCast2D) {
		delete rayCast2D;
		rayCast2D = nullptr;
	}

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

	cEntityManager = CEntityManager::GetInstance();

	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;

	int id = 300;
	if (!clampSides)
		id = 301;

	if (cMap2D->FindValue(id, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	vTransform = glm::i32vec2(uiCol, uiRow);
	oldVTransform = vTransform;

	roundIndex = 0;
	pHealth = 5;

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

	if (!cPhysics2D)
		cPhysics2D = new CPhysics2D;
	if (!collider2D)
		collider2D = new Collider2D;
	//Collider2D
	collider2D->Init(vTransform);

	//Animation is initialised in factory...
	
	//Raycast
	rayCast2D = new RayCast2D;
	rayCast2D->Init(this, cEntityManager->GetAllLivingEntities());
	rayCast2D->SetTarget(cEntityManager->GetPlayer());

	//Physics initialisation
	cPhysics2D->Init(&vTransform);
	mSpd = 5;

	//Smart AI stuff
	maxStateTimer[(int)FSM::IDLE] = int(2.f * (float)cSettings->FPS);
	maxStateTimer[(int)FSM::PATROL] = int(4.f * (float)cSettings->FPS);
	maxStateTimer[(int)FSM::ATTACK] = int(3.f * (float)cSettings->FPS);

	maxIntervalTimer[(int)FSM::IDLE] = int(0.2f * (float)cSettings->FPS);

	sCurrentFSM = RandomiseFSM();
	stateTimer = maxStateTimer[(int)sCurrentFSM];
	intervalTimer = maxIntervalTimer[(int)sCurrentFSM];

	dir = RandomiseDir();

	SaveCurrData();

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;
	inView = false;

	return true;
}

void CMobEnemy2D::SaveCurrData(void) {
	data dStore;
	dStore.currDir = dir;
	dStore.currPos = vTransform;
	dStore.currInterval = intervalTimer;
	dStore.currStatus = sCurrentFSM;
	dStore.currTimer = stateTimer;

	savedData[currFrame] = dStore;
}

void CMobEnemy2D::Attacked(int hp, CPhysics2D* bounceObj) {
	if (pShield > 0) //Return if shield is enabled/ player does not get damaged
		return;

	pHealth = Math::Max(0, pHealth - 1);
	pShield = pMaxShield + 1; //Offset by 1 frame for better synchronisation (FUTURE JEVON IF YOU KNOW YOU KNOW IF NOT THEN LMAO)

	//Collision response between the objects
	if (bounceObj) {
		glm::vec2 ogVel = cPhysics2D->GetVelocity();

		if (vTransform.x > bounceObj->GetPosition().x)
			cPhysics2D->SetVelocity(glm::vec2(-mSpd, ogVel.y));
		else if (vTransform.x < bounceObj->GetPosition().x)
			cPhysics2D->SetVelocity(glm::vec2(mSpd, ogVel.y));

		cPhysics2D->CollisionResponse(bounceObj, 1.5f, 1.5f);
		cPhysics2D->SetBoolKnockBacked(true);
		bounceObj->SetBoolKnockBacked(true);

		DEBUG_MSG("ENEMYHIT!");

		/*float maxSpd = 2.f;
		if (cPhysics2D->GetVelocity().length() > maxSpd)
			cPhysics2D->SetVelocity(glm::normalize(cPhysics2D->GetVelocity()) * maxSpd);*/
	}
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
	//Raycast  test
	rayCast2D->RayCheck();

	if (!inView) { //Do not activate if enemy is out of view
		if (WithinProjectedCamera(cEntityManager->GetPlayer()))
			inView = true;

		return;
	}

	// Store the old position
	oldVTransform = vTransform;

	if (!patrol)
		UpdateDumb(dElapsedTime);
	else
		UpdateSmart(dElapsedTime);

	//Health lives update
	UpdateHealthLives();

	animatedSprites->Update(dElapsedTime);
	currFrame++;
}

void CMobEnemy2D::UpdateSmart(float dElapsedTime) {

}

void CMobEnemy2D::UpdateDumb(float dElapsedTime) {
	//Clamping of position
	ClampPos();

	//Physics
	UpdateMovement((float)dElapsedTime);
	cPhysics2D->Update((float)dElapsedTime);

	//Collision with world's object
	collider2D->position = vTransform;
	CollisionUpdate();

	if (vTransform == oldVTransform && cPhysics2D->GetboolKnockedBacked() == false) {
		if (dir == DIRECTION::LEFT)
			dir = DIRECTION::RIGHT;
		else
			dir = DIRECTION::LEFT;

		cPhysics2D->SetVelocity(glm::vec2(0, 0));
	}
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
			CObject2D* obj = cMap2D->GetCObject((unsigned int)tileCheck.x, (unsigned int)tileCheck.y);

			if (!obj) {
				dir = DIRECTION::RIGHT;
				vTransform = glm::vec2(round(vTransform.x), round(vTransform.y));
			}
			break;
		}

		case DIRECTION::RIGHT: {
			tileCheck.x++;
			CObject2D* obj = cMap2D->GetCObject((unsigned int)tileCheck.x, (unsigned int)tileCheck.y);

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
	cPhysics2D->SetboolGrounded(false);

	int range = 3;
	//Stores nearby objects and its dist to player into a vector 
	vector<pair<CObject2D*, float>> aabbVector;
	for (int row = -range; row <= range; row++) //y
	{
		for (int col = -range; col <= range; col++) //x
		{
			int rowCheck = (int)vTransform.y + row;
			int colCheck = (int)vTransform.x + col;

			if (rowCheck < 0 || colCheck < 0 || rowCheck > cMap2D->GetLevelRow() - 1 || colCheck > cMap2D->GetLevelCol() - 1) continue;
			if (cMap2D->GetCObject(colCheck, rowCheck))
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
	// Detects and Resolves Collsion
	for (auto aabbTuple : aabbVector)
	{
		CObject2D* obj = aabbTuple.first;
		Collision data = (collider2D->CollideWith(obj->GetCollider()));
		if (std::get<0>(data))
		{
			if (obj->GetCollider()->colliderType == Collider2D::ColliderType::COLLIDER_QUAD)
			{
				collider2D->ResolveAABB(obj->GetCollider(), data);

				if (std::get<1>(data) == Direction::UP)
					cPhysics2D->SetboolGrounded(true);
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
		}
	}


	//Player collision
	std::vector<CPlayer2D*> arrPlayer = cEntityManager->GetAllPlayers();

	for (unsigned i = 0; i < arrPlayer.size(); i++) {
		Collider2D* playerCollider = arrPlayer[i]->GetCollider();
		Collision data = (collider2D->CollideWith(playerCollider));

		if (std::get<0>(data)) {
			arrPlayer[i]->Attacked(1,cPhysics2D);

			return;
		}
	}
}

void CMobEnemy2D::UpdateMovement(const float dElapsedTime) {
	if (cPhysics2D->GetboolKnockedBacked()) {
		if (cPhysics2D->GetVelocity() == glm::vec2(0, 0))
			cPhysics2D->SetBoolKnockBacked(false);
		else
			return;
	}

	glm::vec2 velocity = cPhysics2D->GetVelocity();

	if (dir == DIRECTION::LEFT && velocity.x > -mSpd)
		velocity.x = Math::Max(velocity.x - mSpd, -mSpd);
	else if (dir == DIRECTION::RIGHT && velocity.x < mSpd)
		velocity.x = Math::Min(velocity.x + mSpd, mSpd);

	cPhysics2D->SetVelocity(velocity);
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

void CMobEnemy2D::PostRender(void) {
	// Disable blending
	glDisable(GL_BLEND);

	//Raycasting code if needed...
	rayCast2D->RenderRayCast();
}
