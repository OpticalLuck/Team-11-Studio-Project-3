/**
 CPlayer2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include Singleton template
#include "DesignPatterns\SingletonTemplate.h"

//LivingEntity2D
#include "LivingEntity2D.h"

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

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

// Include SoundController
#include "SoundController\SoundController.h"

#include "../InputHandler/CInputHandler.h"

#include <map>
#include <array>
#include "../InputHandler/CInputHandler.h"

#include "Inputs/MouseController.h"

#include "InventoryManager.h"

struct FrameStorage
{
	int iCounter = 0;
	int iCurrentFrame = 0;
	int iStoredFrame = 0;
	int iEndFrame = 0;

	glm::vec2 spawnPos = { 0 , 0 };
};

struct CheckpointState
{
	CInventory* m_CheckpointInventoryState = nullptr;
	float m_CheckpointHP = 3;
	glm::vec2 m_CheckpointPosition = { 0 , 0 };
};

class CPlayer2D : public CEntity2D, public LivingEntity2D
{
public:
	std::vector<std::array<KeyInput, KEYBOARD_INPUTS::KEY_TOTAL>> m_KeyboardInputs;
	std::vector<std::array<MouseInput, MOUSE_INPUTS::MOUSE_TOTAL>> m_MouseInputs;

	CheckpointState m_CheckpointState;
	FrameStorage m_FrameStorage;

	bool bJustTeleported;
	enum class STATE
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

	enum class FACING_DIR
	{
		LEFT = 0,
		RIGHT = 1,
		NUM_DIRECTIONS
	};
	FACING_DIR facing;

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

	int GetCloneID() const;

	void SetKeyInputs(std::vector<std::array<KeyInput, KEYBOARD_INPUTS::KEY_TOTAL>> inputs);
	void SetMouseInputs(std::vector<std::array<MouseInput, MOUSE_INPUTS::MOUSE_TOTAL>> inputs);

	void ResetToCheckPoint();

	void LockWithinBoundary();

	CPlayer2D* const Clone();

	glm::i32vec2 GetCheckpoint(void);

	//Get health
	int GetHealth(void);
	//Get Max health
	int GetMaxHealth(void);

	//Function to call if player gets hit
	void Attacked(int hp = 1, CPhysics2D* bounceObj = nullptr);

	//return cphysics
	CPhysics2D* GetCPhysics(void);

	//returns transform x
	float GetTransformX(void);
	//returns transform y
	float GetTransformY(void);

	CInventory* GetInventory(void);

	bool GetRecording();
	void SetRecording(bool bIsRecording);
protected:
	bool bIsClone;
	int iCloneIndex;

	bool bIsRecording;
	//Timer for actions that need cooldown
	std::pair<bool, double> timerArr[A_TOTAL];

	CInputHandler* cInputHandler;

	glm::vec2 vOldTransform;

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

	// Count the number of jumps
	int jumpCount;
	float fMovementSpeed;
	float fJumpSpeed;

	// Handler to the CSoundController
	CSoundController* cSoundController;

	// Facing direction for rendering and ease of animation
	STATE state;

	// Load a texture
	bool LoadTexture(const char* filename, GLuint& iTextureID);

	void InputUpdate(double dt);

	CInventory* cInventory;
};

