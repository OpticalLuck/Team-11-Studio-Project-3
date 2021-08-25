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

#include "../Library/Source/GameControl/Settings.h"

enum class Direction {
	UP = 0,
	RIGHT = 1,
	DOWN = 2,
	LEFT = 3
};

typedef std::tuple<bool, Direction, glm::vec2> Collision;

class Collider2D
{
private:
	static Collision CheckAABBCollision(Collider2D* obj, Collider2D* target);
	static Collision CheckAABBCircleCollision(Collider2D* aabb, Collider2D* circle);

	// calculates which direction a vector is facing (N,E,S or W)
	static Direction VectorDirection(glm::vec2 target);
public:
	enum class ColliderType
	{
		COLLIDER_QUAD = 0,
		COLLIDER_CIRCLE,
		COLLIDER_TOTAL
	};
	ColliderType colliderType;
	// Attributes of the Shader
	glm::vec2 position;
	glm::vec2 offset;

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

	bool Init(glm::vec2 position, glm::vec2 vec2Dimensions = glm::vec2(0.5f,0.5f), ColliderType colliderType = Collider2D::ColliderType::COLLIDER_QUAD);
	void SetOffset(glm::vec2 offsetVec);

	virtual void SetLineShader(const std::string & name);

	// PreRender
	virtual void PreRender(void);
	// Render
	virtual void Render(void);
	// PostRender
	virtual void PostRender(void);

	Collision CollideWith(Collider2D* object);

	//Just Here if want to use
	// NOT RECOMMENDED IF THERE IS MULTIPLE ENTITIES CLOSE TO EACHOTHER
	void ResolveAABB(Collider2D* object, Collision data);
	void ResolveAABBCircle(Collider2D* object, Collision data, ColliderType target = ColliderType::COLLIDER_CIRCLE);


	bool GetbEnabled() const;
	void SetbEnabled(bool bEnabled);

	glm::vec2 GetPosition() const;
	void SetPosition(glm::vec2 position);
	
	static glm::vec2 ConvertDirectionToVec2(Direction direction);
	void SetAngle(float ang);
protected:

	std::string sLineShaderName;
	unsigned int VAO, VBO;

	float angle;

	//Handlers
	CSettings* cSettings;
	bool bEnabled;
};

