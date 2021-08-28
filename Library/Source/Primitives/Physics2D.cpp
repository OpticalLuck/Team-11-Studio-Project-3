/**
 CPhysics2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Physics2D.h"

#include <iostream>
using namespace std;

#include "../GameControl/Settings.h"
#include "../Math/MyMath.h"

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

		//glm::vec2 friction = glm::normalize(velocity * -1.f);
		//friction *= (coefficient * glm::vec2(0,1));
		//friction /= mass;

		//return friction;
	}

	return glm::vec2(0.f, 0.f);
}
/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CPhysics2D::CPhysics2D(void)
	: position(NULL)
	, v2Gravity(glm::vec2(0.f,-20.f))
	, velocity(glm::vec2(0.f))
	, force(glm::vec2(0.f))
	, mass(1)
	, MAX_SPEED(10.f)
	, FRICTONAL_COEFFICIENT(2.f)
	, bGrounded(false)
	, bKnockBacked(false)
	, sCurrentStatus(STATUS::IDLE)
{ 
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CPhysics2D::~CPhysics2D(void)
{
	position = nullptr;
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
	glm::vec2 a(0.f);
	
	a += CalculateAcceleration();
	a += v2Gravity;

	if (bGrounded)
	{
		glm::vec2 friction = CalculateFriction(FRICTONAL_COEFFICIENT);

		if (abs(friction.x * (float)dElapsedTime) < abs(velocity.x))
			a += friction;
		else
			velocity.x = 0; 
	}
	velocity += a * (float)dElapsedTime;

	//Comment out FOR NOW!!!
	velocity.x = Math::Clamp(velocity.x, -MAX_SPEED, MAX_SPEED);
	velocity.y = Math::Clamp(velocity.y, -MAX_SPEED, MAX_SPEED);

	if (bGrounded && velocity.y <= 0)
		velocity.y = 0;

	*position += velocity * (float)dElapsedTime;

	force = glm::vec2(0.f);
}

void CPhysics2D::CollisionResponse(CPhysics2D* object, float scaleObj1, float scaleObj2) {
	glm::vec2 prevVel1 = velocity;
	glm::vec2 prevVel2 = object->GetVelocity();

	//v2 = ((2 * m1) / (m1 + m2)) * u1 - ((m1 - m2) / (m1 + m2)) * u2
	//v1 = ((m1 - m2) / (m1 + m2)) * u1 + ((2 * m2) / (m1 + m2)) * u2

	velocity = ((mass - object->mass) / (mass + object->mass)) * prevVel1 + ((2 * object->mass) / (mass + object->mass)) * prevVel2;
	object->velocity = ((2 * mass) / (mass + object->mass)) * prevVel1 - ((mass - object->mass) / (mass + object->mass)) * prevVel2;

	//Scaling of velocity
	velocity *= scaleObj1;
	object->velocity *= scaleObj2;
}

void CPhysics2D::DoBounce(glm::vec2 normal, float bounciness)
{
	velocity -= (1 + bounciness) * glm::dot(velocity, normal) * normal;
}

void CPhysics2D::SetForce(const glm::vec2 force)
{
	this->force = force;
}

glm::vec2 CPhysics2D::GetPosition(void) {
	return *position;
}

glm::vec2 CPhysics2D::GetForce() const
{
	return force;
}

void CPhysics2D::SetMass(const float mass)
{
	this->mass = mass;
}

void CPhysics2D::SetVelocity(const glm::vec2 velocity)
{
	this->velocity = velocity;
}

void CPhysics2D::SetboolGrounded(bool bGrounded)
{
	this->bGrounded = bGrounded;
}

glm::vec2 CPhysics2D::GetVelocity() const
{
	return velocity;
}

bool CPhysics2D::GetboolGrounded() const
{
	return bGrounded;
}

bool CPhysics2D::GetboolKnockedBacked(void) const {
	return bKnockBacked;
}

void CPhysics2D::SetBoolKnockBacked(bool bKnockBacked) {
	this->bKnockBacked = bKnockBacked;
}

void CPhysics2D::SetGravity(float gravity)
{
	v2Gravity.y = gravity;
}
