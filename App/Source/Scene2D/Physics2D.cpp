/**
 CPhysics2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Physics2D.h"

#include <iostream>
using namespace std;

#include "GameControl/Settings.h"
#include "Math/MyMath.h"

glm::vec2 CPhysics2D::CalculateAcceleration()
{
	return force * (1 / mass);
}
glm::vec2 CPhysics2D::CalculateFriction(float coefficient)
{
	if (abs(velocity.x) > 0.f)
	{
		// f = uN - N = normal force 
		//Dont need to care about the normal that much since tile based so normal mostly going to be y = 1
		float NormalForce = mass * abs(v2Gravity.y);

		float frictionalforce = coefficient * NormalForce;

		glm::vec2 oppositedirection = glm::normalize(velocity * -1.0f);

		//F = MA, A = F/M
		glm::vec2 friction = oppositedirection * (frictionalforce);
		return friction;
	}

	return glm::vec2(0.f, 0.f);
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
	
	cout << velocity.x << ", " << velocity.y << endl;
	glm::vec2 friction = CalculateFriction(FRICTONAL_COEFFICIENT);

	a += friction;

	a += v2Gravity;
	velocity += a * (float)dElapsedTime;


	if (velocity.x < 0.1f && velocity.x > -0.1f)
		velocity.x = 0;

	velocity.x = Math::Clamp(velocity.x, -MAX_SPEED, MAX_SPEED);
	velocity.y = Math::Clamp(velocity.y, -MAX_SPEED, MAX_SPEED);

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
