/**
 CPhysics2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Physics2D.h"

#include <iostream>
using namespace std;

#include "GameControl/Settings.h"
glm::vec2 CPhysics2D::CalculateAcceleration()
{
	return force * (1 / mass);
}
glm::vec2 CPhysics2D::CalculateFriction(float coefficient)
{
	//Dont need to care about the normal that much since tile based so normal mostly going to be y = 1
	float weight = mass * abs(v2Gravity.y);

	glm::vec2 direction = glm::vec2(0.f);
	if(glm::length(velocity) > 0.f && velocity.x != 0)
		direction = glm::normalize(glm::vec2(velocity.x, 0)) * -1.f;

	glm::vec2 friction = direction * (coefficient * weight);
	return friction;
}
/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CPhysics2D::CPhysics2D(void)
	: position(NULL)
	, velocity(glm::vec2(0.f))
	, force(glm::vec2(0.f))
	, mass(1)
{ 
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CPhysics2D::~CPhysics2D(void)
{
}

/**
@brief Init Initialise this instance
*/ 
bool CPhysics2D::Init(glm::vec2* position)
{
	this->position = position;
	return true;
}

void CPhysics2D::Update(double dElapsedTime)
{
	glm::vec2 a = CalculateAcceleration();
	a += CalculateFriction(FRICTONAL_COEFFICIENT);
	a += v2Gravity;

	velocity += a * (float)dElapsedTime;

	//Cap Speed
	if (glm::length(velocity) > MAX_SPEED) 
	{
		//std::cout << "SPEED LIMIT" << std::endl;
		velocity = glm::normalize(velocity) * MAX_SPEED;
	}

	*position += velocity * (float)dElapsedTime;

	force = glm::vec2(0.f);
}

void CPhysics2D::SetForce(const glm::vec2 force)
{
	this->force = force;
}

void CPhysics2D::SetMass(const float mass)
{
	this->mass = mass;
}

void CPhysics2D::SetVelocity(const glm::vec2 velocity)
{
	this->velocity = velocity;
}

glm::vec2 CPhysics2D::GetVelocity() const
{
	return velocity;
}
