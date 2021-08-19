#include "Camera2D.h"
#include "Math/MyMath.h"
#include "System/Debug.h"
#include "GameControl/Settings.h"
#include "Inputs/MouseController.h"

Camera2D::Camera2D(void) 
	: FirstTime(true)
	, pos(0.f, 0.f)
	, targetPos(0.f, 0.f)
	, fZoom(1.f)
	, fTargetZoom(1.f)
	, vMousePosInWindow(0.f)
	, vMousePosConvertedRatio(0.f)
	, vMousePosWorldSpace(0.f)
	, vMousePosRelativeToCamera(0.f)
{
}

Camera2D::~Camera2D(void) {
	//Nothing for now 
}

void Camera2D::Update(float dElapsedTime) {
	if (FirstTime) {
		pos = targetPos;
		fZoom = fTargetZoom;
		FirstTime = false;
		return;
	}

	float spd = 5 * dElapsedTime;

	//Lerp
	pos.x = Math::Lerp(pos.x, targetPos.x, spd);
	pos.y = Math::Lerp(pos.y, targetPos.y, spd);

	fZoom = Math::Lerp(fZoom, fTargetZoom, spd);
	fZoom = Math::Clamp(fZoom, 0.7f, 2.f);
	fTargetZoom = Math::Clamp(fTargetZoom, 0.7f, 2.f);

	CalculateMousePosInWorldSpace();
	// DEBUG_MSG(fZoom);
}

void Camera2D::UpdateTarget(glm::vec2 target) {
	targetPos = target;
}

void Camera2D::UpdateZoom(float fTarget)
{
	fTargetZoom = fTarget;
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

float Camera2D::getZoom()
{
	return fZoom;
}

float Camera2D::getTargetZoom()
{
	return fTargetZoom;
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

glm::vec2 Camera2D::GetCursorPosInWorldSpace(float offset)
{
	return vMousePosRelativeToCamera + glm::vec2(offset);
}

void Camera2D::CalculateMousePosInWorldSpace()
{
	CMouseController* cMouseController = CMouseController::GetInstance();
	CSettings* cSettings = CSettings::GetInstance();

	vMousePosInWindow = glm::vec2(cMouseController->GetMousePositionX(), cSettings->iWindowHeight - cMouseController->GetMousePositionY());
	vMousePosConvertedRatio = glm::vec2(vMousePosInWindow.x - cSettings->iWindowWidth * 0.5, vMousePosInWindow.y - cSettings->iWindowHeight * 0.5);
	vMousePosWorldSpace = glm::vec2(vMousePosConvertedRatio.x / cSettings->iWindowWidth * cSettings->NUM_TILES_XAXIS, vMousePosConvertedRatio.y / cSettings->iWindowHeight * cSettings->NUM_TILES_YAXIS);
	vMousePosRelativeToCamera = Camera2D::GetInstance()->getCurrPos() + vMousePosWorldSpace / Camera2D::GetInstance()->getZoom();
}
