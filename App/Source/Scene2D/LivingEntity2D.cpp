#include "LivingEntity2D.h"

//Necessary includes
#include "GameControl/Settings.h"
#include "Math/MyMath.h"

LivingEntity2D::LivingEntity2D(void) {
	CSettings* cSettings = CSettings::GetInstance();

	pMaxHealth = 1;
	pHealth = 1;

	//Shield
	pMaxShield = int(2.3f * (float)cSettings->FPS);
	pShield = 0;

	//Blink Interval
	pMaxBlinkInterval = int(0.175f * (float)cSettings->FPS);
	pBlinkInterval = 0;

	//Color initialisation (Normal by default)
	currentColor = glm::vec4(1, 1, 1, 1);
	blinkColor = glm::vec4(100, 100, 100, 1);
}

LivingEntity2D::~LivingEntity2D(void) {
	//Do nothing
}

void LivingEntity2D::Attacked(int hp) {
	if (pShield > 0) //Return if shield is enabled/ entity does not get damaged
		return;

	pHealth = Math::Max(0, pHealth - 1);
	pShield = pMaxShield + 1; //Offset by 1 frame for better synchronisation (FUTURE JEVON IF YOU KNOW YOU KNOW IF NOT THEN LMAO)
}

void LivingEntity2D::UpdateHealthLives(void) {
	if (pShield == 0) { //Return if shield is not enabled / player is not hit
		currentColor = glm::vec4(1, 1, 1, 1);
		return;
	}

	pShield--;

	if (pBlinkInterval == 0) { //If blink is 0, toggle it(Blink) and reset timer 
		pBlinkInterval = pMaxBlinkInterval;
		if (currentColor.r == 1)
			currentColor = blinkColor;
		else
			currentColor = glm::vec4(1, 1, 1, 1);
	}
	else {
		pBlinkInterval--;
	}
}
