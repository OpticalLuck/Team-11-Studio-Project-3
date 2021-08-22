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
	ITEM_SHURIKEN = 2,
	ITEM_POTION = 3,
	ITEM_HOOK = 4,
	ITEM_KUNAI = 5,
	ITEM_LIVES = 10,
	INTERACTABLE_SWITCH1_OFF = 11,
	INTERACTABLE_SWITCH1_ON = 12,
	INTERACTABLE_SWITCH2_OFF = 13,
	INTERACTABLE_SWITCH2_ON = 14,
	INTERACTABLE_PORTAL = 16,
	INTERACTABLE_PORTAL_GATE = 17,
	INTERACTABLE_PORTAL_GATE_OFF = 18,
	INTERACTABLE_SPIKES = 20,
	INTERACTABLE_ALTAR = 21,
	TILE_GROUND = 100,
	TILE_WALL_1 = 101,
	TILE_WALL_2 = 102,
	TILE_GRASS = 103,
	TILE_LEFT_GRASS = 104,
	TILE_RIGHT_GRASS = 105,
	TILE_TOP_RIGHT_GROUND = 106,
	TILE_BOULDER = 150,
	OBJECT_TOTAL,
};

class CObject2D : public CEntity2D
{
	public:
		CObject2D(void);
		CObject2D(int);
		~CObject2D(void); //Do nothing for now
		
		bool Init() override;
		void Update(const double dElapsedTime) override;

		int Getvalue() const;
		void SetValue(int value);

		glm::i32vec2 GetCurrentIndex() const;
		void SetCurrentIndex(glm::i32vec2 currentIndex);
	protected:
		int value;

		float width;
		float height;

		//Current Index in map Grid
		glm::i32vec2 currentIndex;
};

