#include "Interactables.h"

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

#include "Primitives/MeshBuilder.h"
#include "../Scene2D/EntityManager.h"

#include <iostream>

Interactables::Interactables(int iTextureID)
	: bInteraction(false)
	, bOriginalInteraction(false)
	, bPreviousFrameInteraction(false)
	, bCloneInteract(false)
	, quad(NULL)
	, iInteractableID(0)
{
	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	this->iTextureID = iTextureID;

	// Initialise vecIndex
	vTransform = glm::i32vec2(0);

	type = ENTITY_TYPE::INTERACTABLES;

	this->sShaderName = "2DShader";
	Init();
}

Interactables::~Interactables(void)
{
	glDeleteVertexArrays(1, &VAO);

	if (quad)
	{
		delete quad;
		quad = nullptr;
	}
}

bool Interactables::Init()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	type = CEntity2D::ENTITY_TYPE::INTERACTABLES;
	quad = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	if (!cPhysics2D)
		cPhysics2D = new CPhysics2D;
	if (!collider2D)
		collider2D = new Collider2D;

	collider2D->Init(vTransform);

	cEntityManager = CEntityManager::GetInstance();
	interactableType = static_cast<INTERACTABLE_TYPE>(iTextureID);
	if (interactableType < DOOR)
	{
		collider2D->SetbEnabled(false);
		if (interactableType == PRESSURE_PLATE)
		{
			collider2D->Init(vTransform, glm::vec2(0.5f, 0.1f), Collider2D::ColliderType::COLLIDER_QUAD);
			collider2D->SetOffset(glm::vec2(0.f, -0.5f));
		}
	}
	else
	{
		// Initialise Doors
		// Interacted doors are open
		if (interactableType == INTERACTABLE_DOOR_OPEN) {
			interactableType = (INTERACTABLE_TYPE)INTERACTABLE_DOOR_CLOSED;
			bInteraction = true;
			collider2D->SetbEnabled(false);
		}
		else {
			collider2D->SetbEnabled(true);
		}

		collider2D->bIsDisplayed = true;
	}

	bOriginalInteraction = bInteraction;
 	return true;
}

void Interactables::Update(const double dElapsedTime)
{
	bool bCollided = false;
	bPreviousFrameInteraction = bInteraction;

	CEntityManager* em = CEntityManager::GetInstance();

	if (this->interactableType == LEVER)
	{
		for (auto& e : em->GetAllPlayers())
		{
			float distance = glm::length(e->vTransform - this->vTransform);
			if (!e->IsClone() || e->m_FrameStorage.iCurrentFrame <= e->m_FrameStorage.iEndFrame)
			{
			
				if (distance < 1 && e->m_KeyboardInputs[e->m_FrameStorage.iCurrentFrame - 1][KEYBOARD_INPUTS::E].bKeyPressed)
				{
					Activate(false, e);
				}
				
			}
		}
	}
	else if (this->interactableType == PRESSURE_PLATE)
	{
		for (auto& e : em->GetAllPlayers())
		{
			float distance = glm::length(e->vTransform - this->vTransform);
			if (distance < 0.5)
			{
				bCollided = true;
			}
		}

		for (auto& e : em->GetallObstacles())
		{
			float distance = glm::length(e->vTransform - this->vTransform);
			if (distance < 0.5)
			{
				bCollided = true;
			}
		}

		Activate(bCollided);
	}
	else if (this->interactableType == PORTAL)
	{
		for (auto& e : em->GetAllPlayers())
		{
			float distance = glm::length(e->vTransform - this->vTransform);
			if (distance < 0.3)
			{
				for (auto* i : em->GetAllInteractables())
				{
					if (i != this && i->iInteractableID == this->iInteractableID)
					{
						if (!this->bJustTeleported)
						{
							e->vTransform = i->vTransform;
							i->bJustTeleported = true;
							this->bJustTeleported = true;
						}
					}
				}
			}
			else
			{
				this->bJustTeleported = false;
			}
		}
	}
	else if (this->interactableType == GATE)
	{
		float distance = glm::length(em->GetPlayer()->vTransform - this->vTransform);
		if (distance < 0.3)
		{
			CMap2D::GetInstance()->SetCurrentLevel(CMap2D::GetInstance()->GetCurrentLevel() + 1);
			std::string nextLevelPath = "Maps/Level_" + std::to_string(CMap2D::GetInstance()->GetCurrentLevel() + 1) + ".csv";
			if (CMap2D::GetInstance()->LoadMap(nextLevelPath, CMap2D::GetInstance()->GetCurrentLevel()) == false)
			{
				DEBUG_MSG("Map Loading failed");
				return;
			}

			unsigned int iRow = -1;
			unsigned int iCol = -1;
			if (CMap2D::GetInstance()->FindValue(1, iRow, iCol))
			{
				em->GetPlayer()->vTransform = glm::vec2(iCol, iRow);
				em->GetPlayer()->m_CheckpointState.m_CheckpointHP = em->GetPlayer()->GetMaxHealth();
				em->GetPlayer()->m_CheckpointState.m_CheckpointInventoryState = new CInventory(*em->GetPlayer()->GetInventory());
				em->GetPlayer()->m_CheckpointState.m_CheckpointPosition = em->GetPlayer()->vTransform;
				CMap2D::GetInstance()->SetMapInfo(iRow, iCol, 0);
			}
		}
	}
	else if (this->interactableType == CHECKPOINT)
	{
		if (!this->bInteraction)
		{
			float distance = glm::length(em->GetPlayer()->vTransform - this->vTransform);
			if (distance < 0.3)
			{
				bInteraction = true;
				em->GetPlayer()->m_CheckpointState.m_CheckpointHP = em->GetPlayer()->GetMaxHealth();
				em->GetPlayer()->m_CheckpointState.m_CheckpointInventoryState = new CInventory(*em->GetPlayer()->GetInventory());
				em->GetPlayer()->m_CheckpointState.m_CheckpointPosition = em->GetPlayer()->vTransform;
				CSoundController::GetInstance()->PlaySoundByID(SOUND_ID::SOUND_SWITCH);
			}
		}

	}
	else if (this->interactableType == COMMON_CHEST || this->interactableType == RARE_CHEST)
	{
		for (auto& e : em->GetAllPlayers())
		{
			float distance = glm::length(e->vTransform - this->vTransform);

			if (e->m_FrameStorage.iCurrentFrame > e->m_KeyboardInputs.size())
				continue;

			if (distance < 1 && e->m_KeyboardInputs[e->m_FrameStorage.iCurrentFrame - 1][KEYBOARD_INPUTS::E].bKeyPressed)
			{
				Activate(true, e);
			}
		}
	}
	else if (this->interactableType == LIVES)
	{
		if (!bInteraction)
		{
			for (auto& e : em->GetAllPlayers())
			{
				float distance = glm::length(e->vTransform - this->vTransform);
				if (distance < 0.3)
				{
					if (e->iLives < e->iMaxLives)
						++e->iLives;

					this->bInteraction = true;
					break;
				}
			}
		}
	}

	/*if (!em->GetPlayer()->GetRecording())
	{
		if (bInteraction != bCloneInteract)
		{
			bCloneInteract = bInteraction;
		}
	}*/

	if (bInteraction != bCloneInteract)
	{
		bCloneInteract = bInteraction;
	}
	
	// 1,1 || 1,0 || 0,1 
	if ((bInteraction && bCloneInteract) || (!bInteraction && bCloneInteract))
	{
		this->iTextureID = this->interactableType + 1;
	}
	else
	{
		this->iTextureID = this->interactableType;
	}
}

void Interactables::PreRender(void)
{
	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

void Interactables::Render(void)
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
		transform = glm::mat4(1.0f);
		transform = glm::translate(transform, glm::vec3(glm::vec2(actualPos.x, actualPos.y),
			0.0f));
		transform = glm::rotate(transform, glm::radians(fRotate), glm::vec3(0.f, 0.f, 1.f));
		transform = glm::scale(transform, glm::vec3(Camera2D::GetInstance()->getZoom()));

		CShaderManager::GetInstance()->activeShader->setMat4("transform", transform);
		CShaderManager::GetInstance()->activeShader->setVec4("runtime_color", currentColor);

		glBindTexture(GL_TEXTURE_2D, CTextureManager::GetInstance()->MapOfTextureIDs.at(iTextureID));
		glBindVertexArray(VAO);
		quad->Render();
		glBindVertexArray(0);
	}
}

void Interactables::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}

void Interactables::SetInteractableID(int id)
{
	iInteractableID = id;
}

int Interactables::GetInteractableID(void) {
	return iInteractableID;
}

bool Interactables::GetInteracted()
{
	return bInteraction;
}

void Interactables::SetInteracted(bool interacted) {
	bInteraction = interacted;
}

/**
@brief Load a texture, assign it a code and store it in MapOfTextureIDs.
@param filename A const char* variable which contains the file name of the texture
*/
bool Interactables::LoadTexture(const char* filename, GLuint& iTextureID)
{
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

void Interactables::UpdateSound(void) {
	switch (interactableType) {
	case DOOR:
		if (bPreviousFrameInteraction != bInteraction)
		{
			if (!bInteraction)
				CSoundController::GetInstance()->PlaySoundByID(SOUND_ID::SOUND_DOOR_OPEN);
			else
				CSoundController::GetInstance()->PlaySoundByID(SOUND_ID::SOUND_DOOR_CLOSE);
		}
		break;
	default:
		break;
	}
}

bool Interactables::Activate(bool interaction, CPlayer2D* player)
{
	switch (interactableType) {
	case LEVER:
		// CSoundController::GetInstance()->PlaySoundByID(SOUND_ID::SOUND_SWITCH);
		ActivateSwitch(player);
		break;
	case PRESSURE_PLATE:
		ActivatePressurePlate(interaction);
		break;
	case COMMON_CHEST:
		if (!bInteraction)
			CSoundController::GetInstance()->PlaySoundByID(SOUND_ID::SOUND_CHEST);

		OpenChest(player, "Bomb", 5);
		OpenChest(player, "Shuriken", 5);
		OpenChest(player, "Potion", 2);
		this->bInteraction = interaction;
		break;
	case RARE_CHEST:
		if (!bInteraction)
			CSoundController::GetInstance()->PlaySoundByID(SOUND_ID::SOUND_CHEST);

		OpenChest(player, "Bomb", 10);
		OpenChest(player, "Shuriken", 10);
		OpenChest(player, "Potion", 3);
		this->bInteraction = interaction;
		break;
	case DOOR:
		this->bInteraction = interaction;
		if (bPreviousFrameInteraction != bInteraction)
		{
			if (!bInteraction)
				CSoundController::GetInstance()->PlaySoundByID(SOUND_ID::SOUND_DOOR_OPEN);
			else
				CSoundController::GetInstance()->PlaySoundByID(SOUND_ID::SOUND_DOOR_CLOSE);
		}
		break;
	default:
		this->bInteraction = interaction;
		break;
	}

	// Loop through the interactables to activate the corresponding Interactable IDs
	if (this->interactableType < DOOR)
	{
		CEntityManager* entManager = CEntityManager::GetInstance();
		std::vector<Interactables*> intArr = entManager->GetInteractablesbyID(iInteractableID);

		bool activated = false;
		for (auto& e : intArr) {
			if (e->interactableType < DOOR && e->bInteraction && e != this) {
				activated = true;
				break;
			}
		}

		for (auto& e : intArr) {
			if (e->interactableType >= DOOR) {
				if (interactableType == LEVER) {
					bool _switch;
					if (bInteraction)
						_switch = !e->bOriginalInteraction;
					else
						_switch = e->bOriginalInteraction;

					e->Activate(_switch);
					e->collider2D->SetbEnabled(!_switch);
				}
				else if (interactableType == PRESSURE_PLATE) {
					if (activated || interaction) {
						/*e->Activate(true);
						e->collider2D->SetbEnabled(false);*/
						e->Activate(!e->bOriginalInteraction);
						e->collider2D->SetbEnabled(e->bOriginalInteraction);
					}
					else {
						e->Activate(e->bOriginalInteraction);
						e->collider2D->SetbEnabled(!e->bOriginalInteraction);
					}
				}
			}
		}
	}

	return true;
}

int Interactables::GetInteractableType(void) {
	return interactableType;
}

bool Interactables::OpenChest(CPlayer2D* player, std::string itemName, int itemCount)
{
	CInventory* inv = player->GetInventory();
	if (!this->bInteraction)
		inv->AddItem(itemName, itemCount);

	return false;
}

bool Interactables::ActivateSwitch(CPlayer2D* player)
{
	if (player->GetRecording())
		this->bCloneInteract = !this->bCloneInteract;
	else
		this->bInteraction = !this->bInteraction;
	
	return false;
}

bool Interactables::ActivatePressurePlate(bool interaction)
{
	this->bInteraction = interaction;
	return false;
}
