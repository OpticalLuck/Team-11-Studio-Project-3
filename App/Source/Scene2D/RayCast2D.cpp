#include "RayCast2D.h"

#include "../Scene2D/Map2D.h"
#include "Primitives/Collider2D.h"
#include "Primitives/Entity2D.h"
#include "RenderControl/ShaderManager.h"
#include "Primitives/MeshBuilder.h"
#include "Primitives/Camera2D.h"
#include "Math/MyMath.h"

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
	cSettings = nullptr;
	camera2D = nullptr;
}

RayCast2D::~RayCast2D(void) {
	delete collider2D;
	collider2D = nullptr;

	client = nullptr;
	cMap2D = nullptr;
	castedEntity = nullptr;
	cSettings = nullptr;
	camera2D = nullptr;

	entityArr.clear();

	//Delete buffers when done
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void RayCast2D::Init(CEntity2D* client, std::vector<CEntity2D*> entityArr) {
	this->client = client;
	this->entityArr = entityArr;

	cMap2D = CMap2D::GetInstance();
	cSettings = CSettings::GetInstance();
	camera2D = Camera2D::GetInstance();

	//Initialise rendering
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

void RayCast2D::SetTarget(CEntity2D* target) {
	castedEntity = target;
}

float RayCast2D::GetAngle(void) {
	glm::vec2 finalVec = currentPoint - originPoint;
	float angle = atan2f(finalVec.y, finalVec.x);
	angle = Math::RadianToDegree(angle);

	while (angle < 0)
		angle += 360;

	return angle;
}

bool RayCast2D::RayCheck(float angCheck) {
	bool rayCast = RayCheck();
	float ang = GetAngle();

	if (rayCast) {
		//bool check = (ang < 180 - angCheck || ang > 180 + angCheck) && (ang > angCheck && ang < 360 - angCheck);
		bool check = (ang <= angCheck || ang >= 360 - angCheck) || (ang >= 180 - angCheck && ang <= 180 + angCheck);

		if (!check)
			rayCast = false;
	}

	return rayCast;
}

bool RayCast2D::RayCheck(float dist, float angCheck) {
	bool cast = false;

	if (angCheck > 0)
		cast = RayCheck(angCheck);
	else
		cast = RayCheck();

	float currDist = glm::length(currentPoint - originPoint);
	if (currDist <= dist)
		return cast;
	else
		return false;

	//float currDist = glm::length(currentPoint - originPoint);
	//if (currDist <= dist) { //If within dist, do raychecking
	//	if (angCheck > 0)
	//		return RayCheck(angCheck);
	//	else
	//		return RayCheck();
	//}
	//else { //If not then screw it la bodoh
	//	return false;
	//}
}

bool RayCast2D::RayCheck(void) {
	//Reset current point and update targetPoint
	originPoint = client->vTransform;
	currentPoint = originPoint;
	if (castedEntity)
		targetPoint = castedEntity->vTransform;

	//Initialise speed, etc
	glm::vec2 dirVec = glm::normalize(targetPoint - originPoint);
	glm::vec2 spdVec = dirVec * 0.5f;

	std::vector<glm::i32vec2> positionChecked; //Areas where the ray cast have already checked

	while (glm::length(targetPoint - currentPoint) > glm::length(spdVec)) {
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

void RayCast2D::SetEntityArr(std::vector<CEntity2D*> entityArr) {
	this->entityArr = entityArr;
}

void RayCast2D::PreRender(void) {
	// Use the shader defined for this class
	CShaderManager::GetInstance()->Use("2DShader");
}

void RayCast2D::Render(void) {


	//Camera init
	glm::vec2 offset = glm::i32vec2((cSettings->NUM_TILES_XAXIS / 2), (cSettings->NUM_TILES_YAXIS / 2));
	glm::vec2 cameraPos = camera2D->getCurrPos();

	glm::vec2 uvStart = cSettings->ConvertIndexToUVSpace(originPoint - cameraPos + offset);
	glm::vec2 uvEnd = cSettings->ConvertIndexToUVSpace(currentPoint - cameraPos + offset);

	CMesh* mesh = CMeshBuilder::GenerateLine(uvEnd, uvStart, glm::vec4(0, 1, 0, 1));

	glBindVertexArray(VAO);
	
	transform = glm::mat4(1.0f);
	transform = glm::scale(transform, glm::vec3(Camera2D::GetInstance()->getZoom()));
	//transform = glm::translate(transform, glm::vec3(0, 0, 0.f));

	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	unsigned int colorLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "runtime_color");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform4f(colorLoc, 0, 1, 0, 0);

	glBindTexture(GL_TEXTURE0, NULL);
	mesh->Render();

	delete mesh;
	mesh = nullptr;
	
	glBindVertexArray(0);
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
