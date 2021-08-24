#include "RayCast2D.h"

#include "../App/Source/Scene2D/Map2D.h"
#include "Collider2D.h"
#include "Entity2D.h"

RayCast2D::RayCast2D(void) {
	originPoint = targetPoint = currentPoint = glm::vec2(0, 0);

	collider2D = new Collider2D();
	collider2D->SetbEnabled(true);
	collider2D->Init(currentPoint, glm::vec2(0.1f, 0.1f));

	currentTarget = nullptr;
}

RayCast2D::~RayCast2D(void) {
	delete collider2D;
	collider2D = nullptr;

	currentTarget = nullptr;
	cMap2D = nullptr;

	entityArr.clear();
}

void RayCast2D::Init(CEntity2D* currentTarget, std::vector<CEntity2D*> entityArr) {
	this->currentTarget = currentTarget;
	this->entityArr = entityArr;

	cMap2D = CMap2D::GetInstance();
}

bool RayCast2D::RayCheck(void) {
	//Reset current point
	currentPoint = originPoint;

	//Initialise speed, etc
	glm::vec2 dirVec = glm::normalize(targetPoint - originPoint);
	glm::vec2 spdVec = dirVec * 0.5f;

	std::vector<glm::i32vec2> positionChecked; //Areas where the ray cast have already checked

	while ((targetPoint - currentPoint).length() > dirVec.length()) {
		currentPoint += spdVec;
		collider2D->SetPosition(currentPoint);

		//Map based position
		glm::i32vec2 posToCheck = glm::i32vec2(round(currentPoint.x), round(currentPoint.y));

		//Check if position has already been checked
		std::vector<glm::i32vec2>::iterator unique = std::find(positionChecked.begin(), positionChecked.end(), posToCheck);
		if (unique != positionChecked.end()) //Ignore if it already has been looped
			continue;
		else
			positionChecked.push_back(posToCheck); //Else, add it into array

		//Entity check
		for (unsigned i = 0; i < entityArr.size(); i++) {
			CEntity2D* entity = entityArr[i];

			if (entity == currentTarget) //Ignore if its the client 
				continue;

			//Collision check
			Collider2D* collided = entity->GetCollider();
			Collision data = collider2D->CollideWith(collided);

			//Collision successful
			if (std::get<0>(data)) {
				if (collided->colliderType == Collider2D::ColliderType::COLLIDER_QUAD)
					collider2D->ResolveAABB(collided, data);
				else if (collided->colliderType == Collider2D::ColliderType::COLLIDER_CIRCLE)
					collider2D->ResolveAABBCircle(collided, data, Collider2D::ColliderType::COLLIDER_QUAD);

				return true;
			}
		}

		//Map based checking
		CObject2D* obj = cMap2D->GetCObject(posToCheck.x, posToCheck.y);

		if (obj && obj != currentTarget) { //If object is available, check object
			Collider2D* collided = obj->GetCollider();
			Collision data = collider2D->CollideWith(collided);

			//Collision successful
			if (std::get<0>(data)) {
				if (collided->colliderType == Collider2D::ColliderType::COLLIDER_QUAD)
					collider2D->ResolveAABB(collided, data);
				else if (collided->colliderType == Collider2D::ColliderType::COLLIDER_CIRCLE)
					collider2D->ResolveAABBCircle(collided, data, Collider2D::ColliderType::COLLIDER_QUAD);

				return true;
			}
		}
	}

	return false;
}
