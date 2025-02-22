#include "Boss2D.h"

// Include Map
#include "Map2D.h"
// Include Mesh Builder
#include "Primitives/MeshBuilder.h"
// Include ImageLoader
#include "System\ImageLoader.h"
//Include Math.h
#include "Math/MyMath.h"
//Include shader manager
#include "RenderControl/ShaderManager.h"
//Include CEntityManager
#include "EntityManager.h"
//Projectiles
#include "Projectiles.h"
//Bullet2D
#include "Bullet2D.h"
//Raycasting
#include "RayCast2D.h"

CBoss2D::CBoss2D(void) {
	bIsActive = false;
	bulletAng = 0;
	cMap2D = nullptr;
	currentColor = glm::vec4();
	pHealth = 0;
	currTarget = nullptr;
	quadMesh = nullptr;
	camera = nullptr;

	fsmIndex = 0;
	bulletTimer = currAtkDuration = currPauseDuration = 0;

	maxBulletTimer[0] = 0;
	maxBulletTimer[1] = 0;
	maxBulletTimer[2] = 0;
	maxBulletTimer[3] = 0;

	pauseEnabled = true;
	isSeen = false;

	minAtkDuration = 3.5f;
	maxAtkDuration = 8.f;

	minPauseDuration = 1.5f;
	maxPauseDuration = 5.f;

	beforeAtkDuration = 0;
	beforeBulletAng = 0;
	beforeIsSeen = false;
	beforePauseDuration = 0;
	savedAtkTimer = savedPauseTimer = 0;

	id = 305;

	//rayCast2D
	rayCast2D = new RayCast2D;
}

CBoss2D::~CBoss2D(void) {
	for (unsigned i = 0; i < arrPlayer.size(); i++)
		arrPlayer[i] = nullptr;

	arrPlayer.clear();

	// We won't delete this since it was created elsewhere
	cMap2D = NULL;

	camera = nullptr;
	cSettings = nullptr;
	currTarget = nullptr;
	cEntityManager = nullptr;

	if (rayCast2D) {
		delete rayCast2D;
		rayCast2D = nullptr;
	}

	if (quadMesh) {
		delete quadMesh;
		quadMesh = nullptr;
	}

	if (mesh) {
		delete mesh;
		mesh = nullptr;
	}

	if (collider2D) {
		delete collider2D;
		collider2D = nullptr;
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

bool CBoss2D::Init(void) {
	cSettings = CSettings::GetInstance();
	cMap2D = CMap2D::GetInstance();
	camera = Camera2D::GetInstance();
	cEntityManager = CEntityManager::GetInstance();

	arrPlayer.clear();
	arrPlayer = cEntityManager->GetAllPlayers();

	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(id, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the boss, so quit this game

	// Erase the value of the boss in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	vTransform = glm::i32vec2(uiCol, uiRow);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS: Create the Quad Mesh using the mesh builder
	if (!quadMesh)
		quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	//CS: Init the color to white
	currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;

	rayCast2D->Init(this);

	if (!cPhysics2D)
		cPhysics2D = new CPhysics2D;
	if (!collider2D)
		collider2D = new Collider2D;

	collider2D->Init(vTransform);
	//Update collider to Boss position
	collider2D->SetPosition(glm::vec3(vTransform, 0.f));

	if (atkTypes.size() == 0) {
		for (int i = 0; i < (int)ATK::A_TOTAL; i++) {
			atkTypes.push_back(ATK(i));
		}
	}

	//Initialisation of variables
	bulletAng = 0;
	beforeBulletAng = bulletAng;

	maxBulletTimer[(int)ATK::A_ATTACK] = int(2.f * float(cSettings->FPS));
	maxBulletTimer[(int)ATK::A_CIRCLE] = int(0.15f * float(cSettings->FPS));
	maxBulletTimer[(int)ATK::A_TWIN] = int(0.3f * float(cSettings->FPS));
	maxBulletTimer[(int)ATK::A_MACHINEGUN] = int(0.15f * float(cSettings->FPS));

	bulletTimer = 0;

	pMaxHealth = 10;
	pHealth = 10;
	currTarget = nullptr;

	isSeen = false;
	beforeIsSeen = isSeen;

	//Array of vectors
	fsmIndex = 0;

	arrATK.push_back((ATK)RandomiseAttack());

	arrAtkDuration.push_back(RandomiseTimer(true));
	currAtkDuration = arrAtkDuration[0];
	beforeAtkDuration = currAtkDuration;

	arrPauseDuration.push_back(RandomiseTimer(false));
	currPauseDuration = arrPauseDuration[0];
	beforePauseDuration = currPauseDuration;

	savedAtkTimer = savedPauseTimer = 0;

	return true;
}

void CBoss2D::Attacked(int hp) {
	pHealth = Math::Max(0, pHealth - 1);
	pShield = pMaxShield + 1; //Offset by 1 frame for better synchronisation (FUTURE JEVON IF YOU KNOW YOU KNOW IF NOT THEN LMAO)
}

void CBoss2D::ResetRecording(void) {
	beforeBulletAng = bulletAng;
	beforeBulletTimer = bulletTimer;

	beforeIsSeen = isSeen;

	beforeTarget = currTarget;

	//Resetting of values
	int currAtkTimer = arrAtkDuration[fsmIndex];
	int currPauseTimer = arrPauseDuration[fsmIndex];
	ATK currAtk = arrATK[fsmIndex];

	beforeAtkDuration = currAtkDuration;
	beforePauseDuration = currPauseDuration;

	arrAtkDuration.clear();
	arrPauseDuration.clear();
	arrATK.clear();

	arrAtkDuration.push_back(currAtkTimer);
	arrPauseDuration.push_back(currPauseTimer);
	arrATK.push_back(currAtk);

	savedAtkTimer = currAtkTimer;
	savedPauseTimer = currPauseTimer;

	fsmIndex = 0;
}

void CBoss2D::ReplayRecording(void) {
	bulletAng = beforeBulletAng;
	bulletTimer = beforeBulletTimer;

	//Check if currtarget exists
	std::vector<CPlayer2D*> playerArr = cEntityManager->GetAllPlayers();

	if (beforeTarget != nullptr) {
		if (beforeTarget == cEntityManager->GetPlayer())
			currTarget = playerArr.back();
		else if (std::count(playerArr.begin(), playerArr.end(), beforeTarget))
			currTarget = beforeTarget;
		else
			currTarget = nullptr;
	}
	else {
		currTarget = nullptr;
	}

	isSeen = beforeIsSeen;

	ResetCurrTimers();

	currAtkDuration = beforeAtkDuration;
	currPauseDuration = beforePauseDuration;

	//Loading of values
	fsmIndex = 0;

	arrAtkDuration[fsmIndex] = savedAtkTimer;
	arrPauseDuration[fsmIndex] = savedPauseTimer;
}

void CBoss2D::SetMaxAtkDuration(float val) {
	maxAtkDuration = val;
}

void CBoss2D::SetMinAtkDuration(float val) {
	minAtkDuration = val;
}

void CBoss2D::SetID(int id) {
	this->id = id;
}

CBoss2D::ATK CBoss2D::RandomiseAttack(void) {
	int randomIndex = Math::RandIntMinMax(0, atkTypes.size() - 1);
	return atkTypes[randomIndex];

	//return ATK(Math::RandIntMinMax(0, (int)ATK::A_TOTAL - 1));
}

int CBoss2D::RandomiseTimer(bool atk) {
	float value = 0; //In terms of seconds first

	if (!atk && !pauseEnabled) //If pause is not enabled, return 0 seconds for pause
		return (int)value;

	if (atk)
		value = Math::RandFloatMinMax(minAtkDuration, maxAtkDuration);
	else
		value = Math::RandFloatMinMax(minPauseDuration, maxPauseDuration);

	return int(value * (float)cSettings->FPS); //Convert to in terms of frames
}

void CBoss2D::SetMaxPauseDuration(float val) {
	maxPauseDuration = val;
}

void CBoss2D::SetMinPauseDuration(float val) {
	minPauseDuration = val;
}

float CBoss2D::GetTransformX(void)
{
	return vTransform.x;
}

float CBoss2D::GetTransformY(void)
{
	return vTransform.y;
}

void CBoss2D::SetAtkTypes(std::vector<CBoss2D::ATK> atkTypes) {
	this->atkTypes = atkTypes;
}

void CBoss2D::SetPauseEnabled(bool pauseEnabled) {
	this->pauseEnabled = pauseEnabled;
}

bool CBoss2D::LoadTexture(const char* filename, GLuint& iTextureID) {
	// Variables used in loading the texture
	int width, height, nrChannels;

	// texture 1
	// ---------
	glGenTextures(1, &iTextureID);
	glBindTexture(GL_TEXTURE_2D, iTextureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	CImageLoader* cImageLoader = CImageLoader::GetInstance();
	unsigned char* data = cImageLoader->Load(filename, width, height, nrChannels, true);
	if (data)
	{
		if (nrChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if (nrChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		// Generate mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		return false;
	}
	// Free up the memory of the file data read in
	free(data);

	return true;
}

void CBoss2D::Update(const double dElapsedTime) {
	if (!bIsActive)
		return; //Return if boss is not active

	//Initialisation of raycast
	rayCast2D->SetEntityArr(cEntityManager->GetAllSolidEntities());

	arrPlayer = cEntityManager->GetAllPlayers();

	bulletTimer = Math::Max(bulletTimer - 1, 0);

	if (currTarget) {
		UpdateAttack((float)dElapsedTime);
		if (arrAtkDuration[fsmIndex] == 0)
			ShuffleNextAttack();

		rayCast2D->SetTarget(currTarget);
		bool currSee = rayCast2D->RayCheck();

		if (!WithinProjectedCamera(currTarget)) {
			ResetCurrTimers();
			DEBUG_MSG("BOSS: Going pack to passive...");
			currTarget = nullptr;
		}

		if (!currSee && currTarget) { //Obtain new target if curr target cannot be seen
			CPlayer2D* newPlayer = GetProbableTarget(cEntityManager->GetAllPlayers(), (float)cMap2D->GetLevelLimit().x);
			if (newPlayer)
				currTarget = newPlayer;
		}
	}
	else {
		//Check if player is near and enable boss fight
		isSeen = false;
		//currTarget = GetNearestTarget((float)cMap2D->GetLevelLimit().x);
		currTarget = GetProbableTarget(cEntityManager->GetAllPlayers(), (float)cMap2D->GetLevelLimit().x);

		if (currTarget) {
			bulletAng = GetAngle(currTarget->vTransform);
		}
	}

	//Health lives update
	UpdateHealthLives();
}

CPlayer2D* CBoss2D::GetProbableTarget(std::vector<CPlayer2D*> arr, float dist) {
	for (unsigned i = 0; i < arr.size(); i++) {
		CPlayer2D* player = arr[i];

		rayCast2D->SetTarget(player);
		bool casted = rayCast2D->RayCheck(dist,0.f);

		if (casted)
			return player;
	}

	return nullptr;
}

void CBoss2D::ResetCurrTimers(void) {
	arrPauseDuration[fsmIndex] = currPauseDuration;
	arrAtkDuration[fsmIndex] = currAtkDuration;
}

void CBoss2D::ShuffleNextAttack(void) {
	ResetCurrTimers();
	DEBUG_MSG("BOSS: Shuffling to next attack...");



	if (recording) {
		fsmIndex++;
		if (arrPauseDuration.size() == fsmIndex) { //Pushback next values
			arrPauseDuration.push_back(RandomiseTimer(false));
		}

		if (arrAtkDuration.size() == fsmIndex) {
			arrAtkDuration.push_back(RandomiseTimer(true));
		}

		if (arrATK.size() == fsmIndex) {
			arrATK.push_back(RandomiseAttack());
		}
	}
	else if (arrPauseDuration.size() > unsigned(fsmIndex + 1)) {
		fsmIndex++;
	}
	else {
		arrPauseDuration[fsmIndex] = RandomiseTimer(false);
		arrAtkDuration[fsmIndex] = RandomiseTimer(true);

		arrATK[fsmIndex] = RandomiseAttack();
	}

	currAtkDuration = arrAtkDuration[fsmIndex];
	currPauseDuration = arrPauseDuration[fsmIndex];
}

void CBoss2D::UpdateAttack(float dElapsedTime) {
	//Remove pause time if its 
	if (isSeen == false && arrPauseDuration[fsmIndex] != 0) {
		DEBUG_MSG("BOSS: Attacking!");

		isSeen = true;
		arrPauseDuration[fsmIndex] = 0;
	}

	if (arrPauseDuration[fsmIndex] > 0) {
		arrPauseDuration[fsmIndex]--;

		if (arrATK[fsmIndex] == ATK::A_ATTACK)
			bulletAng = GetAngle(currTarget->vTransform);

		if (arrATK[fsmIndex] == ATK::A_MACHINEGUN) {
			float targetEnemyAng = GetAngle(currTarget->vTransform);
			float angSpd = 180.f * dElapsedTime;

			if (targetEnemyAng - bulletAng > angSpd)
				bulletAng += angSpd;
			else if (targetEnemyAng - bulletAng < -angSpd)
				bulletAng -= angSpd;
		}

		return; //Dont attack if still in pause state
	}

	//Attack phrase
	arrAtkDuration[fsmIndex] = Math::Max(arrAtkDuration[fsmIndex] - 1, 0); //Reduce timer

	//Rotate direction that bullet will move towards
	switch (arrATK[fsmIndex]) {
		case ATK::A_MACHINEGUN:
		case ATK::A_ATTACK: {
				float targetEnemyAng = GetAngle(currTarget->vTransform);
				float angSpd = 60.f * dElapsedTime;

				if (targetEnemyAng - bulletAng > angSpd)
					bulletAng += angSpd;
				else if (targetEnemyAng - bulletAng < -angSpd)
					bulletAng -= angSpd;
				break;
			}

		case ATK::A_CIRCLE:
		case ATK::A_TWIN:
		{
			float angSpd = 60.f * dElapsedTime;

			bulletAng -= angSpd;
			break;
		}

		default:
			DEBUG_MSG(" ARRATK ENUM IS UNRECOGNISED...");
			break;
	}

	if (bulletTimer > 0)
		return; //Do not fire bullet if timer is not triggered

	bulletTimer = maxBulletTimer[(int)arrATK[fsmIndex]];

	//Spawn bullet
	switch (arrATK[fsmIndex]) {
		case ATK::A_MACHINEGUN:
		case ATK::A_ATTACK: {
			Bullet2D* bullet = ObjectFactory::CreateBullet(bulletAng, vTransform);
			cEntityManager->PushBullet(bullet, cMap2D->GetCurrentLevel());

			break;
		}

		case ATK::A_CIRCLE: {
			for (int i = 0; i < 4; i++) {
				Bullet2D* bullet = ObjectFactory::CreateBullet(bulletAng + (i * 90), vTransform);
				cEntityManager->PushBullet(bullet, cMap2D->GetCurrentLevel());
			}

			break;
		}

		case ATK::A_TWIN: {
			for (int i = 0; i < 2; i++) {
				Bullet2D* bullet = ObjectFactory::CreateBullet(bulletAng + (i * 180), vTransform);
				cEntityManager->PushBullet(bullet, cMap2D->GetCurrentLevel());
			}

			break;
		}

		default:
			break;
	}
}

void CBoss2D::PreRender(void) {
	if (!bIsActive)
		return;

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

void CBoss2D::Render(void) {
	if (!bIsActive)
		return;

	glBindVertexArray(VAO);
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	unsigned int colorLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "runtime_color");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	//Get camera transforms and use them instead
	glm::vec2 offset = glm::vec2((cSettings->NUM_TILES_XAXIS / 2.f), (cSettings->NUM_TILES_YAXIS / 2.f));
	glm::vec2 cameraPos = camera->getCurrPos();

	glm::vec2 IndexPos = vTransform;

	glm::vec2 actualPos = IndexPos - cameraPos + offset;
	actualPos = cSettings->ConvertIndexToUVSpace(actualPos) * Camera2D::GetInstance()->getZoom();

	float clampOffset = cSettings->ConvertIndexToUVSpace(CSettings::AXIS::x, 1, false);
	clampOffset = (clampOffset + 1);

	float clampX = 1.0f + clampOffset;
	float clampY = 1.0f + clampOffset;
	if (actualPos.x <= -clampX || actualPos.x >= clampX || actualPos.y <= -clampY || actualPos.y >= clampY)
		return; //Exit code if enemy is too far to be rendered

	transform = glm::translate(transform, glm::vec3(actualPos.x, actualPos.y, 0.f));
	transform = glm::scale(transform, glm::vec3(Camera2D::GetInstance()->getZoom()));

	/*if (facing == LEFT) //ROTATION CODE: UNCOMMENT IF NEEDED
		transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));*/

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

void CBoss2D::PostRender(void) {
	if (!bIsActive)
		return;

	// Disable blending
	glDisable(GL_BLEND);

	if (cSettings->bShowCollider)
		rayCast2D->RenderRayCast();
}
