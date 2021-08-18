#pragma once
#include "Primitives\Entity2D.h"
#include ".\Command\Command.h"

class Interactables : public CEntity2D
{
public:

	Interactables(void);

	// Destructor
	virtual ~Interactables(void);

	// Init
	bool Init(void);

	// Reset
	bool Reset(void);

	// Update
	void Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

private:

	Command

};

