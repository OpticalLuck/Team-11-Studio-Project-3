#pragma once
#include "Enemy2D.h"
#include "Primitives/SpriteAnimation.h"

class CMobEnemy2D : public CEnemy2D
{
	public:
		CMobEnemy2D(void);
		~CMobEnemy2D(void);

		bool Init(void);

		// Update
		void Update(const double dElapsedTime);

		//Render
		void Render(void);

	protected:
		CSpriteAnimation* animatedSprites;
};

