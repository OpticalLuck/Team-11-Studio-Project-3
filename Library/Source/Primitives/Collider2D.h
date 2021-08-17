#pragma once

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include <string>

class Collider2D
{
public:
	enum CorrectedAxis
	{
		X,
		Y
	};
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

	bool colliderEnabled;

	Collider2D();
	virtual ~Collider2D();

	bool Init();

	virtual void SetLineShader(const std::string & name);

	bool CollideWith(Collider2D* object);

	//Just Here if want to use
	// NOT RECOMMENDED IF THERE IS MULTIPLE ENTITIES CLOSE TO EACHOTHER
	CorrectedAxis ResolveCollision(Collider2D* object);

	// USE THIS IF THERE IS MULTIPLE ENTITES
	void ResolveCollisionX(Collider2D* object);
	void ResolveCollisionY(Collider2D* object);

	// PreRender
	virtual void PreRender(void);
	// Render
	virtual void Render(void);
	// PostRender
	virtual void PostRender(void);
protected:
	std::string sLineShaderName;
	unsigned int VAO, VBO;


};

