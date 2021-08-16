#include "Camera2D.h"

Camera2D::Camera2D(void) {
	FirstTime = true;
	pos = targetPos = glm::vec2(0.f, 0.f);
}

Camera2D::~Camera2D(void) {
	//Nothing for now 
}

void Camera2D::Update(float dElapsedTime) {
	if (FirstTime) {
		pos = targetPos;
		FirstTime = false;
		return;
	}

	float spd = 5 * dElapsedTime;

	//Lerp
	pos.x = pos.x + spd * (targetPos.x - pos.x);
	pos.y = pos.y + spd * (targetPos.y - pos.y);
}

void Camera2D::UpdateTarget(glm::vec2 target) {
	targetPos = target;
}

void Camera2D::Reset(void) {
	FirstTime = true;
}

bool Camera2D::IsFirstTime(void) {
	return FirstTime;
}

glm::vec2 Camera2D::getCurrPos(void) {
	return pos;
}
