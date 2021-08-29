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
#include "../Library/Source/Primitives/Entity2D.h"

//Centity manager
class CEntityManager;

enum OBJECT_TYPE
{
	//CONSUMABLES 2 - 5
	PLAYER_TILE						 = 1,
									 
	CONSUMABLES_START				 = 1,
	CONSUMABLES_POTION,				 
	CONSUMABLES_TOTAL				 = 5,
									 
	//PROJECTILES - 6 - 10			 
	PROJECTILES_START				 = 6,
	PROJECTILES_BOMB				 = 8,
	PROJECTILES_TOTAL				 = 10,
									 
	//BULLETS - 11 -15				 
	BULLETS_START					 = 11,
	BULLETS_KUNAI					 = 12,
	BULLETS_SHURIKEN				 = 13,
	BULLETS_ENEMY					 = 14,
	BULLETS_TOTAL					 = 15,
									 
	EQUIPMENTS_START				 = 16,
	EQUIPMENTS_HOOK					 = 17,
	EQUIPMENTS_TOTAL				 = 20,
	//UI 16 - 25					 
	UI_START						 = 21,
	UI_LIVES						 = 22,
	UI_TOTAL						 = 25,
									 
	//INTERACTABLES 26 - 40			 
	INTERACTABLE_START				 = 26,
	INTERACTABLE_SWITCH1_OFF			 ,
	INTERACTABLE_SWITCH1_ON				 ,
	INTERACTABLE_PRESSURE_PLATE_OFF		 ,
	INTERACTABLE_PRESSURE_PLATE_ON		 ,
	INTERACTABLE_PORTAL					 ,
	INTERACTABLE_PORTAL_GATE			 ,
	INTERACTABLE_PORTAL_GATE_OFF		 ,
	INTERACTABLE_SPIKES					 ,
	INTERACTABLE_ALTAR					 ,
	INTERACTABLE_COMMON_CHEST_CLOSED	 ,
	INTERACTABLE_COMMON_CHEST_OPENED	 ,
	INTERACTABLE_RARE_CHEST_CLOSED		 ,
	INTERACTABLE_RARE_CHEST_OPENED		 ,
	INTERACTABLE_CHECKPOINT_INACTIVE	 ,
	INTERACTABLE_CHECKPOINT_ACTIVE		 ,
	INTERACTABLE_LIVES					 ,

	INTERACTABLE_DOOR_CLOSED         = 50,
	INTERACTABLE_DOOR_OPEN				 ,
	INTERACTABLE_FUTURE_DOOR_CLOSED		 ,
	INTERACTABLE_FUTURE_DOOR_OPEN		 ,
									 
	INTERACTABLE_TOTAL					 ,
									 
	// STATIC TILES 100 - 149		 
	TILE_START						 = 100,
	TILE_GROUND						 = 101,
	TILE_WALL_1						 = 102,
	TILE_WALL_2						 = 103,
	TILE_GRASS						 = 104,
	TILE_LEFT_GRASS					 = 105,
	TILE_RIGHT_GRASS				 = 106,
	TILE_TOP_RIGHT_GROUND			 = 107,
	TILE_DARK						 = 108,
	TILE_FUTURE_1					 = 109,
	TILE_FUTURE_2						  ,
	TILE_FUTURE_3						  ,
	TILE_FUTURE_4						  ,
	TILE_FUTURE_5						  ,
	TILE_FUTURE_6						  ,
	TILE_FUTURE_7						  ,
	TILE_FUTURE_8						  ,
	TILE_FUTURE_FENCE_1					  ,
	TILE_FUTURE_FENCE_2					  ,
	TILE_FUTURE_FENCE_3					  ,
	TILE_VILLEGE_1                        ,
	TILE_VILLEGE_2                        ,
	TILE_VILLEGE_3                        ,
	TILE_VILLEGE_4                        ,
	TILE_VILLEGE_5                        ,
	TILE_VILLEGE_6                        ,
	TILE_VILLEGE_7                        ,
	TILE_VILLEGE_8                        ,
	TILE_VILLEGE_9                        ,
	TILE_VILLEGE_10                       ,
	TILE_VILLEGE_11                       ,
	TILE_VILLEGE_12                       ,
	TILE_VILLEGE_13                       ,
	TILE_VILLEGE_14                       ,
	TILE_VILLEGE_15                       ,
	TILE_VILLEGE_16                       ,
	TILE_VILLEGE_17                       ,
	TILE_VILLEGE_18                       ,
	TILE_TOTAL						 = 149,
									 
	// MOVING TILES 150 - 200		 
	OBSTACLES_START					 = 150,
	OBSTACLE_BOULDER					  ,
	OBSTACLE_FUTURE_BARREL1				  ,
	OBSTACLE_FUTURE_BARREL2				  ,
	OBSTACLE_FUTURE_BOX					  ,
	OBSTACLES_TOTAL					 = 200,
									 
	// ENEMIES 300 - 400			 
	ENEMIES_START					 = 299,
	ENEMY_1							 = 300,
	ENEMY_2							 = 301,
	ENEMY_3							 = 302,
	ENEMY_4							 = 303,
	ENEMY_5							 = 304,
	BOSS							 = 305,
	ENEMIES_TOTAL					 = 306,

	OBJECT_TOTAL,
};

class CObject2D : public CEntity2D
{
	public:
		CObject2D(int itextureID = 0);
		virtual ~CObject2D(void);
		
		virtual bool Init();
		void Update(const double dElapsedTime) override;

		glm::i32vec2 GetCurrentIndex() const;
		void SetCurrentIndex(glm::i32vec2 currentIndex);

		void SetObjectID(int objectID);
		int GetObjectID(void);

		std::vector<pair<CObject2D*, float>>CheckMapCollision(int range = 2);

		virtual void ResolveMapCollision(std::vector<pair<CObject2D*, float>> aabbvector);
		virtual void ResolveEnemyCollision();
		virtual void ResolvePlayerCollision();
	protected:
		float width;
		float height;

		//Current Index in map Grid
		glm::i32vec2 currentIndex;

		//Object ID
		int ObjectID;

		bool bCollided;

		//Handlers
		CEntityManager* cEntityManager;
};

