#include "RayCast2D.h"

#include "../App/Source/Scene2D/Map2D.h"
#include "../Library/Source/Primitives/Collider2D.h"
#include "Primitives/Entity2D.h"
#include "RenderControl/ShaderManager.h"

RayCast2D::RayCast2D(void) {
	originPoint = targetPoint = currentPoint = glm::vec2(0, 0);

	collider2D = new Collider2D();
	collider2D->SetbEnabled(true);
	collider2D->Init(currentPoint, glm::vec2(0.1f, 0.1f));

	transform = glm::mat4();

	VAO = VBO = 0;

	client = nullptr;
	cMap2D = nullptr;
	castedEntity = nullptr;
}

RayCast2D::~RayCast2D(void) {
	delete collider2D;
	collider2D = nullptr;

	client = nullptr;
	cMap2D = nullptr;
	castedEntity = nullptr;

	entityArr.clear();

	//Delete buffers when done
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void RayCast2D::Init(CEntity2D* currentTarget, std::vector<CEntity2D*> entityArr) {
	this->client = currentTarget;
	this->entityArr = entityArr;

	cMap2D = CMap2D::GetInstance();

	//Initialise rendering
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	float vertices[] = {
		-1,0,0,1,0,
		1,0,0,1,0
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

bool RayCast2D::RayCheck(void) {
	//Reset current point and update targetPoint
	currentPoint = originPoint;
	targetPoint = castedEntity->vTransform;

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

			if (entity == client) //Ignore if its the client 
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

				if (entity == castedEntity)
					return true;
				else
					return false;
			}
		}

		//Map based checking
		CObject2D* obj = cMap2D->GetCObject(posToCheck.x, posToCheck.y);

		if (obj && obj != client) { //If object is available, check object
			Collider2D* collided = obj->GetCollider();
			Collision data = collider2D->CollideWith(collided);

			//Collision successful
			if (std::get<0>(data)) {
				if (collided->colliderType == Collider2D::ColliderType::COLLIDER_QUAD)
					collider2D->ResolveAABB(collided, data);
				else if (collided->colliderType == Collider2D::ColliderType::COLLIDER_CIRCLE)
					collider2D->ResolveAABBCircle(collided, data, Collider2D::ColliderType::COLLIDER_QUAD);

				if (obj == castedEntity)
					return true;
				else
					return false;
			}
		}
	}

	return true;
}

void RayCast2D::PreRender(void) {
	// Use the shader defined for this class
	CShaderManager::GetInstance()->Use("LineShader");
}

void RayCast2D::Render(void) {
	//Camera init
	glm::vec2 offset = glm::vec2(float(CSettings::GetInstance()->NUM_TILES_XAXIS / 2.f), float(CSettings::GetInstance()->NUM_TILES_YAXIS / 2.f));

	glm::vec2 dirVec = (currentPoint - originPoint);
	dirVec = glm::normalize(dirVec);

	//Necessary values
	float angle = atan2f(dirVec.y, dirVec.x);
	glm::vec2 position = (currentPoint + originPoint) * 0.5f;
	float scale = (currentPoint - originPoint).length();

	glm::vec2 cameraPos = Camera2D::GetInstance()->getCurrPos();

	glm::vec2 objCamPos = position - cameraPos + offset;

	glm::vec2 actualPos = CSettings::GetInstance()->ConvertIndexToUVSpace(objCamPos);

	float clampOffset = CSettings::GetInstance()->ConvertIndexToUVSpace(CSettings::AXIS::x, 1, false) / 2;
	clampOffset = (clampOffset + 1);

	float clampX = 1.0f + clampOffset;
	float clampY = 1.0f + clampOffset;
	//if (fabs(actualPos.x) < clampX && fabs(actualPos.y) < clampY)
	{
		transform = glm::mat4(1.f);
		transform = glm::translate(transform, glm::vec3(actualPos.x, actualPos.y, 0.f));
		transform = glm::rotate(transform, angle, glm::vec3(0, 0, 1));
		transform = glm::scale(transform, glm::vec3(CSettings::GetInstance()->TILE_WIDTH * scale, CSettings::GetInstance()->TILE_HEIGHT, 1.f));
		CShaderManager::GetInstance()->activeShader->setMat4("transform", transform);

		// render box
		glBindVertexArray(VAO);
		glDrawArrays(GL_LINE_LOOP, 0, 2);
	}
}

void RayCast2D::PostRender(void) {
	//Render bounding box
	collider2D->PreRender();
	collider2D->Render();
	collider2D->PostRender();
}

void RayCast2D::RenderRayCast(void) {
	PreRender();
	Render();
	PostRender();
}
