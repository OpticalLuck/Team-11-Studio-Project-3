#pragma once

#include "Object2D.h"
#include "EntityManager.h"

//physics
#include "Physics2D.h"

class CMap2D;

class EnemyBullet2D : public CObject2D
{
	public:
		EnemyBullet2D(void);
		virtual ~EnemyBullet2D(void);

		bool Init(float angle, glm::vec2 spawnPoint);
		
		//Overridden functions
		void Update(const double dElapsedTime);
		void Render(void);

		bool OutOfWorld(void);
		int GetHealth(void);

	protected:
		glm::vec2 dirVec;

		float mAngle;
		float mForce; 
		float mScale; //Scale from 0 to 1 (1 being the max tile width and height)

		int pHealth;

		//Handler 
		CMap2D* cMap2D;

		//Collision check
		void CollisionCheck(void);
};

