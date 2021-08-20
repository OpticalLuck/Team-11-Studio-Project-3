#pragma once
#include "Enemy2D.h"
#include "EntityManager.h"


class EnemyBullet2D : public CEnemy2D
{
	public:
		EnemyBullet2D(void);
		virtual ~EnemyBullet2D(void);

		bool Init(float angle, glm::vec2 spawnPoint);
		
		//Overridden functions
		void Update(const double dElapsedTime);
		void Render(void);

		bool OutOfWorld(void);

	protected:
		glm::vec2 dirVec;

		float mAngle;
		float mSpd; 
		float mScale; //Scale from 0 to 1 (1 being the max tile width and height)

		//Collision check
		void CollisionCheck(void);
};

