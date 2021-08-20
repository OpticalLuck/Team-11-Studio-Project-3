/**
 CEnemy2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include shader
#include "RenderControl\shader.h"

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include CEntity2D
#include "Primitives/Entity2D.h"

// Include the Map2D as we will use it to check the player's movements and actions
class CMap2D;
class CEntityManager;

// Include Settings
#include "GameControl\Settings.h"

// Include Physics2D
#include "Physics2D.h"

// Include Player2D
#include "Player2D.h"

#include "Math/MyMath.h"

//#include "EntityManager.h"


class CEnemy2D : public CEntity2D
{
public:
	// Constructor
	CEnemy2D(void);

	// Destructor
	virtual ~CEnemy2D(void);

	// Init
	virtual bool Init(void);

	// Update
	virtual void Update(const double dElapsedTime);

	// PreRender
	virtual void PreRender(void);

	// Render
	virtual void Render(void);

	// PostRender
	virtual void PostRender(void);

	// Set the indices of the enemy2D
	void SetTransform(const int iIndex_XAxis, const int iIndex_YAxis);

	// Get Health
	int GetHealth() const;

	// boolean flag to indicate if this enemy is active
	bool bIsActive;

protected:
	enum class DIRECTION
	{
		LEFT = 0,
		RIGHT = 1,
		NUM_DIRECTIONS
	};

	enum FSM
	{
		IDLE = 0,
		PATROL,
		ATTACK,
		HIT,
		RUN,
		DEATH,
		NUM_FSM
	};

	//Direction
	DIRECTION dir;

	//CS: The quadMesh for drawing the tiles
	CMesh* quadMesh;

	// Handler to the CMap2D instance
	CMap2D* cMap2D;

	//Handler to camera
	Camera2D* camera;

	CEntityManager* cEntityManager;

	// A transformation matrix for controlling where to render the entities
	glm::mat4 transform;

	// The vec2 variable which stores the UV coordinates to render the enemy2D
	glm::vec2 vec2UVCoordinate;

	// Settings
	CSettings* cSettings;

	// Physics
	//CPhysics2D cPhysics2D;

	// Current color
	glm::vec4 currentColor;

	//Player target and arrays handling it...
	CPlayer2D* currTarget;
	std::vector<CPlayer2D*> arrPlayer;

	//Get angle from enemy to position
	float GetAngle(glm::vec2 pos);


	// Current FSM
	FSM sCurrentFSM;

	//Current round
	int roundIndex;

	//Initial direction for that round
	DIRECTION roundDir[5];

	// Store health for combat
	int health;

	// Load a texture
	bool LoadTexture(const char* filename, GLuint& iTextureID);

	//Get nearest player. NOTE: TARGET HAS TO BE VALID AND BE WITHIN RANGE FIRST. DEFAULT ACCEPTABLE RANGE IS WITHIN HALF CAMERA WIDTH
	CPlayer2D* GetNearestTarget(float dist = 16.f);

	float GetDistanceBetweenPlayer(CPlayer2D* player);

	//Check if its within the projected camera of the player entity
	bool WithinProjectedCamera(CPlayer2D* player);

	//Randomise direction
	DIRECTION RandomiseDir(void);
};

