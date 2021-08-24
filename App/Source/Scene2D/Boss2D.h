#pragma once

// Include shader
#include "RenderControl\shader.h"

//CEnemy2D
#include "Enemy2D.h"

// Include the Map2D as we will use it to check the player's movements and actions
class CMap2D;

//Include CEntityManager to get curr round
class CEntityManager;

//Include Player2D
class CPlayer2D;

//Factory 
#include "../App/Source/Factory/ObjectFactory.h"

// Include Settings
#include "GameControl\Settings.h"

//Math.h
#include "Math/MyMath.h"

class CBoss2D : public CEnemy2D
{
	public :
		enum class ATK {
			A_CIRCLE, //Attacks in a circular pattern.
			A_TWIN, //Circular pattern but only 2
			A_ATTACK, //Shoots at the player normally
			A_MACHINEGUN, //Constantly targets the player like a machine gun would
			A_TOTAL
		};

		CBoss2D(void);
		virtual ~CBoss2D(void);

		bool Init(void);

		// Update
		void Update(const double dElapsedTime);

		// PreRender
		void PreRender(void);

		// Render
		void Render(void);

		// PostRender
		void PostRender(void);

		//Set how many types of attacks the enemy can do
		void SetAtkTypes(std::vector<ATK> atkTypes);

		//Set pauseenabled
		void SetPauseEnabled(bool pauseEnabled);

		//Set id
		void SetID(int id);

		//Set min and max AtkDuration
		void SetMaxAtkDuration(float val);
		void SetMinAtkDuration(float val);

		//Set min and max pauseDuration
		void SetMaxPauseDuration(float val);
		void SetMinPauseDuration(float val);

	protected:
		int id;

		std::vector<ATK>* arrATK; //Current attack during current round
		int fsmIndex; //Current index of where its at in the array

		//AtkDuration (Based on frames)
		int currAtkDuration;
		//Min and max of atkDuration (Based on seconds)
		float minAtkDuration;
		float maxAtkDuration;

		//pauseDuration (Based on frames)
		int currPauseDuration;
		//Min and max of pauseDuration (Based on seconds)
		float minPauseDuration;
		float maxPauseDuration;

		std::vector<int>* arrAtkDuration;
		std::vector<int>* arrPauseDuration;

		bool isSeen;

		float bulletAng; //Angle of which where the bullet will come from
		int maxBulletTimer[(int)ATK::A_TOTAL]; //Timer in terms of frames
		int bulletTimer;

		bool pauseEnabled;
		std::vector<ATK> atkTypes; //How many attack types does it store

		//Rendering
		//Mesh
		CMesh* quadMesh;

		//Factory
		ObjectFactory factory;

		// Current color
		bool bIsActive;

		//Functions
		// Load a texture
		bool LoadTexture(const char* filename, GLuint& iTextureID);

		//Randomise timer
		int RandomiseTimer(bool atk);

		//Randomise attacks
		ATK RandomiseAttack(void);

		//Update attacks
		void UpdateAttack(float dElapsedTime);

		//Shuffle to next attack
		void ShuffleNextAttack(void);

		//Reset current values
		void ResetCurrTimers(void);
};

