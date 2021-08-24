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

		//Set clamp sides
		void SetClampSlides(bool clamp);

		//Setter and getter of animated sprites
		void SetAnimatedSprites(CSpriteAnimation* animatedSprites);
		CSpriteAnimation* GetAnimatedSprites(void);

	protected:
		glm::vec2 oldVTransform;
		float mSpd;

		CSpriteAnimation* animatedSprites;

		bool clampSides; //check if enemy can jump off cliff and stuff
		bool inView;

		void UpdateMovement(const float dElapsedTime);
		void CollisionUpdate(void);
		void ClampPos(void);
};

