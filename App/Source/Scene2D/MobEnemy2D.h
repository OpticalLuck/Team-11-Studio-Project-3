#pragma once
#include "Enemy2D.h"
#include "Primitives/SpriteAnimation.h"

class RayCast2D;

class CMobEnemy2D : public CEnemy2D
{
	public:
		struct data {
			FSM currStatus;

			glm::vec2 currPos;
			DIRECTION currDir;

			int currTimer;
			int currInterval; //Patrolling
		};

		std::map<int, data> savedData;

		CMobEnemy2D(void);
		~CMobEnemy2D(void);

		bool Init(void);

		// Update
		void Update(const double dElapsedTime);

		//Render
		void Render(void);
		void PostRender(void);

		//Set clamp sides
		void SetClampSlides(bool clamp);

		//Setter and getter of animated sprites
		void SetAnimatedSprites(CSpriteAnimation* animatedSprites);
		CSpriteAnimation* GetAnimatedSprites(void);

		//Function to call if Entity gets hit
		virtual void Attacked(int hp = 1, CPhysics2D* bounceObj = nullptr);

		void SaveCurrData(void);

	protected:
		glm::vec2 oldVTransform;
		float mSpd;

		CSpriteAnimation* animatedSprites;

		bool patrol;
		bool clampSides; //check if enemy can jump off cliff and stuff
		bool inView;

		void UpdateMovement(const float dElapsedTime);
		void CollisionUpdate(void);
		void ClampPos(void);

		void UpdateDumb(float dElapsedTime);
		void UpdateSmart(float dElapsedTime);

		//Raycasting client
		RayCast2D* rayCast2D;

		//Smart ai
		int stateTimer;
		int maxStateTimer[(int)FSM::NUM_FSM];

		int intervalTimer;
		int maxIntervalTimer[(int)FSM::NUM_FSM];
};

