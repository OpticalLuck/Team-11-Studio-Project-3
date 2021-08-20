#pragma once

// Include GLEW
#ifndef GLEW_STATIC
#include <Windows.h>
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include <string>
#include <tuple>

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

typedef std::tuple<bool, Direction, glm::vec2> Collision;

class CSettings;

class Collider2D
{
private:
	static Collision CheckAABBCollision(Collider2D* obj, Collider2D* target);
	static Collision CheckAABBCircleCollision(Collider2D* aabb, Collider2D* circle);

	// calculates which direction a vector is facing (N,E,S or W)
	static Direction VectorDirection(glm::vec2 target)
	{
		glm::vec2 compass[] = {
			glm::vec2(0.0f, 1.0f),	// up
			glm::vec2(1.0f, 0.0f),	// right
			glm::vec2(0.0f, -1.0f),	// down
			glm::vec2(-1.0f, 0.0f)	// left
		};
		float max = 0.0f;
		unsigned int best_match = -1;
		for (unsigned int i = 0; i < 4; i++)
		{
			float dot_product = glm::dot(glm::normalize(target), compass[i]);
			if (dot_product > max)
			{
				max = dot_product;
				best_match = i;
			}
		}
		return (Direction)best_match;
	}
public:
	enum ColliderType
	{
		COLLIDER_QUAD = 0,
		COLLIDER_CIRCLE,
		COLLIDER_TOTAL
	};
	ColliderType colliderType;
	// Attributes of the Shader
	glm::vec2 position;
	glm::mat4 transform;

	// Bounding Box specifications
	glm::vec2 vec2Dimensions;
	// Bounding Box colour
	glm::vec4 vec4Colour;

	// Boolean flag to indicate if Bounding Box is displayed
	bool bIsDisplayed;
	// Line width
	float fLineWidth;

	Collider2D();
	virtual ~Collider2D();

	bool Init();

	virtual void SetLineShader(const std::string & name);

	Collision CollideWith(Collider2D* object);

	//Just Here if want to use
	// NOT RECOMMENDED IF THERE IS MULTIPLE ENTITIES CLOSE TO EACHOTHER
	void ResolveAABB(Collider2D* object, Direction axis);

	void ResolveAABBCircle(Collider2D* object, Collision data, ColliderType target = COLLIDER_CIRCLE);

	// PreRender
	virtual void PreRender(void);
	// Render
	virtual void Render(void);
	// PostRender
	virtual void PostRender(void);


	bool GetbEnabled() const;
	void SetbEnabled(bool bEnabled);

	glm::vec2 GetPosition() const;
	void SetPosition(glm::vec2 position);
protected:
	std::string sLineShaderName;
	unsigned int VAO, VBO;

	//Handlers
	CSettings* cSettings;
	bool bEnabled;
};

