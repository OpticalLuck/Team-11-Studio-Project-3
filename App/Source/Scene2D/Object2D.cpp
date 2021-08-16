#include "Object2D.h"

CObject2D::CObject2D(void) {
	value = 0;
	collidable = true;
	worldSpace = indexSpace = glm::i32vec2();
}

CObject2D::CObject2D(int value, bool collidable) {
	this->value = value;
	this->collidable = collidable;

	worldSpace = indexSpace = glm::i32vec2();
}

CObject2D::~CObject2D(void) {
	//Do nothing for now
}

void CObject2D::setIndexSpace(glm::i32vec2 indexSpace) {
	this->indexSpace = indexSpace;
}

void CObject2D::setWorldSpace(glm::i32vec2 worldSpace) {
	this->worldSpace = worldSpace;
}
