#include "Object2D.h"

CObject2D::CObject2D(void) {
	value = 0;
	indexSpace = glm::vec2();

	width = height = 1.f; //Default value is 1 x 1
}

CObject2D::CObject2D(int value) {
	this->value = value;

	indexSpace = glm::i32vec2();
}

CObject2D::~CObject2D(void) {
	//Do nothing for now
}

glm::vec2 CObject2D::GetIndexSpace()
{
	return indexSpace;
}

void CObject2D::SetIndexSpace(glm::vec2 indexSpace) {
	this->indexSpace = indexSpace;
}

int CObject2D::Getvalue() const
{
	return value;
}

void CObject2D::SetValue(int value)
{
	this->value = value;
}
