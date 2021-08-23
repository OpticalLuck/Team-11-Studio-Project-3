#pragma once

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

#include "Object2D.h"

class Interactables : public CObject2D
{
public:

	enum INTERACTABLE_TYPE
	{
		BUTTON = 201,
		LEVER = 202,
		PRESSURE_PLATE = 203,
	};

	INTERACTABLE_TYPE interactableType;

	Interactables(int iTextureID = 0);

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

	void SetInteractableID(int);

private:

	int iInteractableID;

	CMesh* quad;

	// Current color
	glm::vec4 currentColor;

	// Boolean to check if the interactable is currently being interacted with
	bool bInteraction;

	bool LoadTexture(const char* filename, GLuint& iTextureID);
};

