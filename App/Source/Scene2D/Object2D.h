#pragma once

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

enum OBJECT_TYPE
{
	//CONSUMABLES 2 - 5
	CONSUMABLES_START				= 1,
	CONSUMABLES_POTION,
	CONSUMABLES_TOTAL				= 5,

	//PROJECTILES - 6 - 10
	PROJECTILES_START				= 6,
	PROJECTILES_SHURIKEN			= 7,
	PROJECTILES_TOTAL				= 10,

	//BULLETS - 11 -15
	BULLETS_START					= 11,
	BULLETS_KUNAI					= 12,
	BULLETS_TOTAL					= 15,
	
	EQUIPMENTS_START				= 16,
	EQUIPMENTS_HOOK					= 17,
	EQUIPMENTS_TOTAL				= 20,
	//UI 16 - 25
	UI_START						= 21,
	UI_LIVES						= 22,
	UI_TOTAL						= 25,

	//INTERACTABLES 26 - 40
	INTERACTABLE_START				= 26,
	INTERACTABLE_SWITCH1_OFF		= 27,
	INTERACTABLE_SWITCH1_ON			= 28,
	INTERACTABLE_SWITCH2_OFF		= 28,
	INTERACTABLE_SWITCH2_ON			= 29,
	INTERACTABLE_PORTAL				= 28,
	INTERACTABLE_PORTAL_GATE		= 30,
	INTERACTABLE_PORTAL_GATE_OFF	= 31,
	INTERACTABLE_SPIKES				= 32,
	INTERACTABLE_ALTAR				= 33,
	INTERACTABLE_TOTAL				= 39,

	// STATIC TILES 100 - 149
	TILE_START						= 100,
	TILE_GROUND						= 101,
	TILE_WALL_1						= 102,
	TILE_WALL_2						= 103,
	TILE_GRASS						= 104,
	TILE_LEFT_GRASS					= 105,
	TILE_RIGHT_GRASS				= 106,
	TILE_TOP_RIGHT_GROUND			= 107,
	TILE_TOTAL						= 149,

	// MOVING TILES 150 - 200
	OBSTACLES_START					= 150,
	OBSTACLE_BOULDER				= 151,
	OBSTACLES_TOTAL					= 200,

	// ENEMIES 300 - 400
	OBJECT_TOTAL,
};

class CObject2D : public CEntity2D
{
	public:
		CObject2D(int itextureID = 0);
		virtual ~CObject2D(void);
		
		bool Init() override;
		void Update(const double dElapsedTime) override;

		glm::i32vec2 GetCurrentIndex() const;
		void SetCurrentIndex(glm::i32vec2 currentIndex);

	protected:
		float width;
		float height;

		//Current Index in map Grid
		glm::i32vec2 currentIndex;
};

