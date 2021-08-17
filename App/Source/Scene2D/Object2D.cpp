#include "Object2D.h"

CObject2D::CObject2D(void) {
	value = 0;
	collidable = true;
	indexSpace = glm::vec2();

	width = height = 1.f; //Default value is 1 x 1
}

CObject2D::CObject2D(int value, bool collidable) {
	this->value = value;
	this->collidable = collidable;

	indexSpace = glm::i32vec2();
}

CObject2D::~CObject2D(void) {
	//Do nothing for now
}

void CObject2D::setIndexSpace(glm::vec2 indexSpace) {
	this->indexSpace = indexSpace;
}
