#include "Object2D.h"
#include <iostream>

#include "EntityManager.h"

CObject2D::CObject2D(int iTextureID) 
	: width(1.f)
	, height(1.f)
{
	this->iTextureID = iTextureID;

	vTransform = glm::i32vec2();
	currentIndex = glm::i32vec2();

	cEntityManager = nullptr;
}

CObject2D::~CObject2D(void) {
	cEntityManager = nullptr; //Dellocation of handlers
}

bool CObject2D::Init()
{
	if (!collider2D)
		collider2D = new Collider2D;

	//Collider2D initialisation
	collider2D->Init(vTransform);
	collider2D->SetbEnabled(true);

	cEntityManager = CEntityManager::GetInstance();

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
