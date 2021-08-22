/**
 CPhysics2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

class CPhysics2D
{
private:
	glm::vec2 CalculateAcceleration();
public:	
	enum STATUS
	{
		IDLE = 0,
		JUMP,
		FALL,
		HOLD,
		NUM_STATUS
	};

	// Constructor
	CPhysics2D(void);

	// Destructor
	virtual ~CPhysics2D(void);

	// Init
	bool Init(glm::vec2* position);

	// Update
	void Update(double dElapsedTime);

	glm::vec2 CalculateFriction(float coefficient);

	void DoBounce(glm::vec2 normal, float bounciness = 1.f);

	void SetForce(const glm::vec2 force);
	glm::vec2 GetForce() const;
	void SetMass(const float mass);
	void SetVelocity(const glm::vec2 velocity);
	void SetboolGrounded(bool bGrounded);
	glm::vec2 GetVelocity() const;
	bool GetboolGrounded() const;

	void SetGravity(float gravity);

	float MAX_SPEED;
	float FRICTONAL_COEFFICIENT;
protected:
	glm::vec2 v2Gravity = glm::vec2(0.f, -20.f);
	glm::vec2 *position;
	glm::vec2 velocity;
	glm::vec2 force;
	float mass;
	STATUS sCurrentStatus;

	bool bGrounded;
};

