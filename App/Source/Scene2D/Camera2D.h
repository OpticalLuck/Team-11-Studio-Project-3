#pragma once

// Include Singleton template
#include "DesignPatterns\SingletonTemplate.h"

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
#include "Primitives/Entity2D.h"

class Camera2D : public CSingletonTemplate<Camera2D>
{
	friend CSingletonTemplate<Camera2D>;

	public:
		Camera2D(void);
		~Camera2D(void);

		void Update(float dElapsedTime);
		void UpdateTarget(glm::i32vec2 target);
		void Reset(void);

		bool IsFirstTime(void);
	private:
		bool FirstTime;
		glm::i32vec2 pos; //Position set to index space for now because worldspace can suck my ching cheng han ji
		glm::i32vec2 targetPos;
};

