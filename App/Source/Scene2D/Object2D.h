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

class CObject2D : public CEntity2D
{
	public:
		CObject2D(void);
		CObject2D(int);
		~CObject2D(void); //Do nothing for now
		
		bool Init() override;
		void Update(const double dElapsedTime) override;

		int Getvalue() const;
		void SetValue(int value);

		glm::i32vec2 GetCurrentIndex() const;
		void SetCurrentIndex(glm::i32vec2 currentIndex);
	protected:
		int value;

		float width;
		float height;

		//Current Index in map Grid
		glm::i32vec2 currentIndex;
};

