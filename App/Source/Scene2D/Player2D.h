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
class Camera2D;

// Include Keyboard controller
#include "Inputs\KeyboardController.h"

#include "Inputs\MouseController.h"

// Include Physics2D
#include "Physics2D.h"

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"


// Include SoundController
#include "..\SoundController\SoundController.h"

#include "../InputHandler/CInputHandler.h"

#include <map>
#include <array>
#include "../InputHandler/CInputHandler.h"

#include "Inputs/MouseController.h"

#include "InventoryManager.h"

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
	STATE m_playerState;

	enum ACTION
	{
		A_JUMP = 0,
		A_ATTACK,
		A_TOTAL,
	};


	// Constructor
	CPlayer2D(void);

	// Destructor
	virtual ~CPlayer2D(void);

	// Init
	bool Init(void);

	bool Init(glm::i32vec2 spawnpoint, int iCloneIndex);

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

	void SetClone(bool bIsClone);

	bool IsClone();

	void SetKeyInputs(std::vector<std::array<KeyInput, KEYBOARD_INPUTS::KEY_TOTAL>> inputs);
	void SetMouseInputs(std::vector<std::array<MouseInput, MOUSE_INPUTS::MOUSE_TOTAL>> inputs);

	void ResetToCheckPoint();

	void LockWithinBoundary();

	CPlayer2D* const Clone();

	glm::i32vec2 GetCheckpoint(void);

protected:
	bool bIsClone;
	
	//Timer for actions that need cooldown
	std::pair<bool, double> timerArr[A_TOTAL];

	enum DIRECTION
	{
		LEFT = 0,
		RIGHT = 1,
		UP = 2,
		DOWN = 3,
		NUM_DIRECTIONS
	};

	CInputHandler* cInputHandler;
	std::vector<std::array<KeyInput, KEYBOARD_INPUTS::KEY_TOTAL>> m_KeyboardInputs;
	std::vector<std::array<MouseInput, MOUSE_INPUTS::MOUSE_TOTAL>> m_MouseInputs;

	int iTempFrameCounter; // move to game manager/scene2D/PlayGameState later

	glm::vec2 vOldTransform;

	CPhysics2D cPhysics2D;

	glm::i32vec2 checkpoint;

	// Handler to the CMap2D instance
	CMap2D* cMap2D;

	//Handler to the camera instance
	Camera2D* camera;

	// Keyboard Controller singleton instance
	CKeyboardController* cKeyboardController;
	CMouseController* cMouseController;
	//CS: Animated Sprite
	CSpriteAnimation* animatedSprites;

	// Current color
	glm::vec4 currentColor;

	// Count the number of jumps
	int jumpCount;
	float fMovementSpeed;
	float fJumpSpeed;

	// Handler to the CSoundController
	CSoundController* cSoundController;

	// Facing direction for rendering and ease of animation
	DIRECTION facing;
	STATE state;

	// Load a texture
	bool LoadTexture(const char* filename, GLuint& iTextureID);

	// Update the health and lives
	void UpdateHealthLives(void);

	void InputUpdate(double dt);

	CInventory* cInventory;
};

