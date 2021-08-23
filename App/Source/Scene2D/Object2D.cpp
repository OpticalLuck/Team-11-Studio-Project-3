#include "Object2D.h"
#include <iostream>

CObject2D::CObject2D(int iTextureID) 
	: width(1.f)
	, height(1.f)
{
	this->iTextureID = iTextureID;

	vTransform = glm::i32vec2();
	currentIndex = glm::i32vec2();
}

CObject2D::~CObject2D(void) {
	//No Pointers
}

bool CObject2D::Init()
{
	if (!collider2D)
		collider2D = new Collider2D;

	//Collider2D initialisation
	collider2D->Init(vTransform);

	//if (objectID == 0)
		collider2D->SetbEnabled(true);
	//else
		//collider2D->SetbEnabled(false);

	return true;
}

void CObject2D::Update(const double dElapsedTime)
{
	if (collider2D->position != vTransform)
		collider2D->position = vTransform;
}

glm::i32vec2 CObject2D::GetCurrentIndex() const
{
	return currentIndex;
}

void CObject2D::SetCurrentIndex(glm::i32vec2 currentIndex)
{
	this->currentIndex = currentIndex;
}
