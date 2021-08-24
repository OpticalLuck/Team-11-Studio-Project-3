#pragma once

// Include Singleton template
#include "..\DesignPatterns\SingletonTemplate.h"

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
#include "Entity2D.h"

class Camera2D : public CSingletonTemplate<Camera2D>
{
	friend CSingletonTemplate<Camera2D>;

	public:

		Camera2D(void);
		~Camera2D(void);

		void Update(float dElapsedTime);
		void UpdateTarget(glm::vec2 target);
		void MoveTarget(float moveX, float moveY);
		void UpdateZoom(float fTarget);
		void UpdatePos(glm::vec2 pos);
		void Reset(void);

		glm::vec2 getCurrPos(void); 
		glm::vec2 getTarget(void);
		float getZoom();
		float getTargetZoom();
		bool IsFirstTime(void);
		void ClampCamPos(glm::i32vec2 clampPos);
		float GetPosX(void);
		float GetPosY(void);

		glm::vec2 GetCursorPosInWorldSpace(float offset = 0.f);

	private:
		glm::vec2 vMousePosInWindow;
		glm::vec2 vMousePosConvertedRatio;
		glm::vec2 vMousePosWorldSpace;
		glm::vec2 vMousePosRelativeToCamera;

		bool FirstTime;
		glm::vec2 pos; //Position set to index space for now because worldspace can suck my ching cheng han ji
		glm::vec2 targetPos;

		float fZoom;
		float fTargetZoom;

		void CalculateMousePosInWorldSpace();
};

