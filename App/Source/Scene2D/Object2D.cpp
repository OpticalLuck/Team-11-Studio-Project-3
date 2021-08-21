#include "Object2D.h"
#include <iostream>

CObject2D::CObject2D(void) {
	value = 0;
	vTransform = glm::vec2();
	currentIndex = glm::i32vec2();

	width = height = 1.f; //Default value is 1 x 1
}

CObject2D::CObject2D(int value) {
	this->value = value;

	vTransform = glm::i32vec2();
	currentIndex = glm::i32vec2();

	width = height = 1.f; //Default value is 1 x 1
}

CObject2D::~CObject2D(void) {
	//Do nothing for now
}

bool CObject2D::Init()
{
	if (!collider2D)
		collider2D = new Collider2D;

	//Collider2D initialisation
	collider2D->Init();
	collider2D->SetPosition(vTransform);

	if (value >= 100 && value < 300)
		collider2D->SetbEnabled(true);
	else
		collider2D->SetbEnabled(false);

	return true;
}

void CObject2D::Update(const double dElapsedTime)
{
	if (collider2D->position != vTransform)
		collider2D->position = vTransform;
}

int CObject2D::Getvalue() const
{
	return value;
}

void CObject2D::SetValue(int value)
{
	this->value = value;
}

glm::i32vec2 CObject2D::GetCurrentIndex() const
{
	return currentIndex;
}

void CObject2D::SetCurrentIndex(glm::i32vec2 currentIndex)
{
	this->currentIndex = currentIndex;
}
