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

class CMap2D;

class CObject2D : public CEntity2D
{
	friend class CMap2D;

	public:
		CObject2D(void);
		CObject2D(int, bool);
		~CObject2D(void); //Do nothing for now

		void setIndexSpace(glm::vec2);

	private:
		int value;
		bool collidable;

		float width;
		float height;

		glm::vec2 indexSpace; //LOCATION OF OBJECT IN WORLD SPACE
};

