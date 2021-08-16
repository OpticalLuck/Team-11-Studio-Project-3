/**
 CPlayer2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include Singleton template
#include "DesignPatterns\SingletonTemplate.h"

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include CEntity2D
#include "Primitives/Entity2D.h"

// Include the Map2D as we will use it to check the player's movements and actions
class CMap2D;

// Include Keyboard controller
#include "Inputs\KeyboardController.h"

// Include Physics2D
#include "Physics2D.h"

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

// Include InventoryManager
#include "InventoryManager.h"

// Include SoundController
#include "..\SoundController\SoundController.h"

#include "../KeyboardInputHandler/CKeyboardInputHandler.h"

#include <map>
#include <array>
#include "../KeyboardInputHandler/CKeyboardInputHandler.h"

class CPlayer2D : public CEntity2D
{
public:
	enum STATE
	{
		S_IDLE = 0,
		S_MOVE,
		S_JUMP,
		S_DOUBLE_JUMP,
		S_HOLD,
		S_ATTACK,
		S_HIT,
		S_CLIMB,
		S_DEATH,
		S_NUM_STATE
	};

	// Constructor
	CPlayer2D(void);

	// Destructor
	virtual ~CPlayer2D(void);

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

	STATE Getstate() const;

	void Hit(int health);

	void SetClone(bool bIsClone);

	bool IsClone();

	void SetInputs(std::vector<std::array<bool, CKeyboardInputHandler::KEYBOARD_INPUTS::INPUT_TOTAL>> inputs);
protected:

	bool bIsClone;

	enum DIRECTION
	{
		LEFT = 0,
		RIGHT = 1,
		UP = 2,
		DOWN = 3,
		NUM_DIRECTIONS
	};

	CKeyboardInputHandler* cKeyboardInputHandler;
	std::vector<std::array<bool, CKeyboardInputHandler::KEYBOARD_INPUTS::INPUT_TOTAL>> m_CloneKeyboardInputs;

	int iTempFrameCounter; // move to game manager/scene2D/PlayGameState later

	glm::vec2 vOldTransform;

	glm::i32vec2 checkpoint;

	// Handler to the CMap2D instance
	CMap2D* cMap2D;

	// Keyboard Controller singleton instance
	CKeyboardController* cKeyboardController;

	// Physics
	CPhysics2D cPhysics2D;

	//CS: Animated Sprite
	CSpriteAnimation* animatedSprites;

	// Current color
	glm::vec4 currentColor;

	// InventoryManager
	// CInventoryManager* cInventoryManager;
	// InventoryItem
	CInventoryItem* cInventoryItem;

	// Count the number of jumps
	int jumpCount;
	float fMovementSpeed;

	// Handler to the CSoundController
	CSoundController* cSoundController;

	// Timer for Running
	double runtimer;
	// Timer for Jumping
	double jumptimer;
	// Timer for death
	double deathtimer;
	// Timer for attacking
	double attacktimer;
	// Timer for Invulnerbility
	double invulTimer;
	bool bDamaged;
	
	// Facing direction for rendering and ease of animation
	DIRECTION facing;
	STATE state;

	// Load a texture
	bool LoadTexture(const char* filename, GLuint& iTextureID);

	void MovementUpdate(double dt);

	// Update the health and lives
	void UpdateHealthLives(void);

	bool InRangeOfTile(unsigned tileID);
};

