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
	glm::vec2 CalculateFriction(float coefficient);
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

	void SetForce(const glm::vec2 force);
	void SetMass(const float mass);
	void SetVelocity(const glm::vec2 velocity);

	glm::vec2 GetVelocity() const;
protected:
	//const glm::vec2 v2Gravity = glm::vec2(0.f, -20.f);
	const glm::vec2 v2Gravity = glm::vec2(0.f, 0.f);
	const float MAX_SPEED = 10.f;
	const float FRICTONAL_COEFFICIENT = 1.f;
	glm::vec2 *position;
	glm::vec2 velocity;
	glm::vec2 force;
	float mass;
	STATUS sCurrentStatus;
};

