#pragma once
#include "Enemy2D.h"
#include "Primitives/SpriteAnimation.h"

#include <algorithm> 

class RayCast2D;

class CMobEnemy2D : public CEnemy2D
{
	public:
		struct data {
			FSM currStatus;
			glm::vec2 currVel;

			glm::vec2 currPos;
			DIRECTION currDir;

			int currTimer;
			int currInterval; //Patrolling

			CPlayer2D* currPlayer;
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
		std::pair<int, bool> GetNearestDataKey(void);

		std::vector<std::pair<int, CMobEnemy2D::data>> SortSavedData(void);
		static bool DataCheck(std::pair<int, data>& a, std::pair<int, data>& b);

		void LoadCurrData(int frame);
		void SetPatrol(bool);

		void SetID(int id);

		//Recording stuff
		void ResetRecording(void); //Resets the recording

		//Replay
		void ReplayRecording(void);

	protected:
		glm::vec2 oldVTransform;
		glm::vec2 posToChase;
		glm::vec2 spawnPoint;
		float mSpd;

		CSpriteAnimation* animatedSprites;

		bool patrol;
		bool clampSides; //check if enemy can jump off cliff and stuff
		bool inView;
		float distCheck;

		void UpdateMovement(const float dElapsedTime);
		void ChaseEnemyX(void); //Chasing the enemy, in terms of x axis
		void ChaseEnemyY(void); //Chasing the enemy, in terms of y axis
		void UpdateAttack(const float dElapsedTime);

		void CollisionUpdate(void);
		void ClampPos(void);

		void UpdateDumb(float dElapsedTime);
		void UpdateSmart(float dElapsedTime);

		void LockWithinBoundary(void);

		void RandomiseStateTimer(FSM state = FSM::NUM_FSM);
		void RandomiseIntervalTimer(void);

		void OnIntervalTriggered(FSM);
		void OnStateTimerTriggered(FSM);

		void EnableAttack(void); //To enable attack state, USE THIS FUNCTION AND DONT CHANGE MANUALLY AS INSIDE GOT SAVE AND LOAD
		void EnableGoBack(void); //Going back state, when enemy lost player
		void RandomisePassive(void); //Randomise passive state

		std::vector<CPlayer2D*> ResetEntValues(void); //Resetting entity values during runtime ( CALL THIS DURING RUNTIME/ UPDATE ONLY)

		//Raycasting client
		RayCast2D* rayCast2D;

		//Previoslyy seen player
		CPlayer2D* prevPlayer2D;

		//Smart ai
		int stateTimer;
		int maxStateTimer[(int)FSM::NUM_FSM];

		int intervalTimer;

		int id;

		//int nearestFrame;
		std::pair<int, bool> nearestFrame;
};

