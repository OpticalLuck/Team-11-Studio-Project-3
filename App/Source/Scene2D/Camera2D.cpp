#include "Camera2D.h"

Camera2D::Camera2D(void) {
	FirstTime = true;
	pos = targetPos = glm::i32vec2(0, 0);
}

Camera2D::~Camera2D(void) {
	//Nothing for now 
}

void Camera2D::Update(float dElapsedTime) {
	if (FirstTime) {
		pos = targetPos;
		FirstTime = false;
	}

	float spd = 5 * dElapsedTime;

	if ((targetPos - pos).length() < spd * 2) //Dont lerp if player alr reached area
		return;

	//Lerp
	pos.x = pos.x + spd * (targetPos.x - pos.x);
	pos.y = pos.y + spd * (targetPos.y - pos.y);
}

void Camera2D::UpdateTarget(glm::i32vec2 target) {
	targetPos = target;
}

void Camera2D::Reset(void) {
	FirstTime = true;
}

bool Camera2D::IsFirstTime(void) {
	return FirstTime;
}
