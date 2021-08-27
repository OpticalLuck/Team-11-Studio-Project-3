 #pragma once

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"
#include "Player2D.h"
#include "Object2D.h"

	enum INTERACTABLE_TYPE
	{
		// E To Interact
		LEVER = OBJECT_TYPE::INTERACTABLE_SWITCH1_OFF,
		COMMON_CHEST = OBJECT_TYPE::INTERACTABLE_COMMON_CHEST_CLOSED,
		RARE_CHEST = OBJECT_TYPE::INTERACTABLE_RARE_CHEST_CLOSED,

		PRESSURE_PLATE = OBJECT_TYPE::INTERACTABLE_PRESSURE_PLATE_OFF,
		PORTAL = OBJECT_TYPE::INTERACTABLE_PORTAL,
		GATE = OBJECT_TYPE::INTERACTABLE_PORTAL_GATE,

		DOOR = OBJECT_TYPE::INTERACTABLE_DOOR_CLOSED
	};

class Interactables : public CObject2D
{
public:

	bool bJustTeleported = false;

	INTERACTABLE_TYPE interactableType;

	Interactables(int iTextureID = 0);

	// Destructor
	virtual ~Interactables(void);

	// Init
	bool Init();

	// Update
	void Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

	bool Activate(bool interaction = true, CPlayer2D * player = nullptr);
	bool OpenChest(CPlayer2D* player, std::string itemName, int itemCount);
	bool ActivateSwitch(CPlayer2D* player);
	bool ActivatePressurePlate(bool);
	void SetInteractableID(int);
	bool GetInteracted();

private:

	int iInteractableID;

	CMesh* quad;

	// Current color
	glm::vec4 currentColor;

	// Boolean to check if the interactable is currently being interacted with
	bool bInteraction;
	bool bCloneInteract;
	bool LoadTexture(const char* filename, GLuint& iTextureID);
};

