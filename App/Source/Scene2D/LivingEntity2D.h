#pragma once

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

class LivingEntity2D
{
	public:
		LivingEntity2D(void);
		~LivingEntity2D(void);

		void UpdateHealthLives(void);
		virtual void Attacked(int hp = 1);

	protected:
		//Health and stuff
		int pHealth;
		int pMaxHealth;

		int pShield; //Current time of how long player will be protected for (in terms of frames)
		int pMaxShield; //Maximum time of how long player will be protected for (in terms of frames)

		int pBlinkInterval; //Interval of the flashing color
		int pMaxBlinkInterval; //Max interval

		// Current color
		glm::vec4 currentColor;

		//Intensity
		glm::vec4 blinkColor;
};

