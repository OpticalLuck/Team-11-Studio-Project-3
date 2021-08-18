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

void Camera2D::UpdatePos(glm::vec2 pos)
{
	this->pos = pos;
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

glm::vec2 Camera2D::getTarget(void)
{
	return targetPos;
}

void Camera2D::ClampCamPos(glm::i32vec2 clampPos) {
	CSettings* cSettings = CSettings::GetInstance();
	float xOffset = ((float)cSettings->NUM_TILES_XAXIS / 2.f) - 1;
	float yOffset = ((float)cSettings->NUM_TILES_YAXIS / 2.f) - 1;

	//Clamping of X axis
	if (pos.x < xOffset)
		pos.x = xOffset;
	else if (pos.x > clampPos.x - xOffset - 2)
		pos.x = clampPos.x - xOffset - 2;

	//Clamping of Y axis
	if (pos.y < yOffset)
		pos.y = yOffset;
	else if (pos.y > clampPos.y - yOffset - 2)
		pos.y = clampPos.y - yOffset - 2;
}
