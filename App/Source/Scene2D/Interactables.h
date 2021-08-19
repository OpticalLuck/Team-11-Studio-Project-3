#pragma once
#include "..\App\Source\Command\Command.h"

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

#include "Object2D.h"

class Interactables : public CObject2D
{
public:

	enum INTERACTABLE_TYPE
	{
		//Collidables
		BOULDER = 110,

		//Non-collidables
		BUTTON = 201,
		LEVER = 202,
		PRESSURE_PLATE = 203,
	};

	INTERACTABLE_TYPE interactableType;

	Interactables(void);

	// Destructor
	virtual ~Interactables(void);

	// Init
	bool Init(int iMapID);

	// Update
	void Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

	void SetCommand(COMMANDS eCommand);

private:

	CSpriteAnimation* animatedSprites;

	// Current color
	glm::vec4 currentColor;

	// Command to run when interacted with
	Command cCommand;

	// Boolean to check if the interactable is currently being interacted with
	bool bInteracted;

	bool LoadTexture(const char* filename, GLuint& iTextureID);
};

