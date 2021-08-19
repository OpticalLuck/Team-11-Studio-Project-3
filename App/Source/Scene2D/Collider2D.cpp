#include "Collider2D.h"

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

#include <GLFW/glfw3.h>
#include "GameControl/Settings.h"
#include "Camera2D.h"

//#include "DesignPatterns/SingletonTemplate.h"

Collision Collider2D::CheckAABBCollision(Collider2D* obj, Collider2D* target)
{
	float threshold = 0.01f;
	//vec2Dimensions is half width and half height	
	bool collisionX = abs(obj->position.x - target->position.x) <= obj->vec2Dimensions.x + target->vec2Dimensions.x - threshold;
	bool collisionY = abs(obj->position.y - target->position.y) <= obj->vec2Dimensions.y + target->vec2Dimensions.y;

	return std::make_tuple(collisionX && collisionY, UP, glm::vec2(0.0f, 0.0f));
}

Collision Collider2D::CheckAABBCircleCollision(Collider2D* aabb, Collider2D* circle)
{
	// get difference vector between both centers
	glm::vec2 difference = circle->position - aabb->position;
	glm::vec2 clamped = glm::clamp(difference, -aabb->vec2Dimensions, aabb->vec2Dimensions);
	// now that we know the the clamped values, add this to AABB_center and we get the value of box closest to circle
	glm::vec2 closest = aabb->position + clamped;
	// now retrieve vector between center circle and closest point AABB and check if length < radius
	difference = closest - circle->position;

	if (glm::length(difference) < circle->vec2Dimensions.x) // not <= since in that case a collision also occurs when object one exactly touches object two, which they are at the end of each collision resolution stage.
		return std::make_tuple(true, VectorDirection(difference), difference);
	else
		return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

Collider2D::Collider2D()
	: vec2Dimensions(glm::vec2(0.5f, 0.5f))
	, position(glm::vec3(1.f))
	, vec4Colour(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f))
	, bIsDisplayed(true)
	, fLineWidth(1.0f)
	, cSettings(NULL)
	, bEnabled(true)
{
	sLineShaderName = "LineShader";
	colliderType = COLLIDER_QUAD;
}

Collider2D::~Collider2D()
{
}

bool Collider2D::Init()
{
	float vertices[] = {
		-vec2Dimensions.x, -vec2Dimensions.y, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec2Dimensions.x, -vec2Dimensions.y, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec2Dimensions.x, vec2Dimensions.y, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec2Dimensions.x, vec2Dimensions.y, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		-vec2Dimensions.x, vec2Dimensions.y, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		-vec2Dimensions.x, -vec2Dimensions.y, vec4Colour.x, vec4Colour.y, vec4Colour.z,
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	glLineWidth(fLineWidth);

	cSettings = CSettings::GetInstance();
	 
	return true;
}

void Collider2D::SetLineShader(const std::string& name)
{
	sLineShaderName = name;
}

Collision Collider2D::CollideWith(Collider2D* object)
{
	if (object->bEnabled)
	{
		if (object->colliderType == COLLIDER_QUAD)
		{
			return CheckAABBCollision(this, object);
		}
		else if (object->colliderType == COLLIDER_CIRCLE)
		{
			return CheckAABBCircleCollision(this, object);
		}

	}
	
	return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

void Collider2D::ResolveAABB(Collider2D* object, CorrectedAxis axis)
{
	glm::vec2 direction = object->position - position;

	float shortestXDist = 10000;
	float shortestYDist = 10000;
	for (int i = -1; i <= 1; i++)
	{
		//-1 and 1 
		if (i != 0)
		{
			float tempxdist = glm::length(glm::vec2(position.x + vec2Dimensions.x * i, 0) - glm::vec2(object->position.x + object->vec2Dimensions.x * -i, 0));
			if (tempxdist < shortestXDist)
				shortestXDist = tempxdist;

			float tempydist = glm::length(glm::vec2(0, position.y + vec2Dimensions.y * i) - glm::vec2(0, object->position.y + object->vec2Dimensions.y * -i));
			if (tempydist < shortestYDist)
				shortestYDist = tempydist;
		}
	}

	if (axis == X)
	{
		if (shortestXDist < shortestYDist && shortestYDist != 0)
		{
			glm::vec2 correctionAxis = glm::normalize(glm::vec2(direction.x * -1, 0.f));
			position += glm::vec2(shortestXDist, 0) * correctionAxis;
		}
	}
	else if (axis == Y)
	{
		if (shortestXDist > shortestYDist)
		{
			glm::vec2 correctionAxis = glm::normalize(glm::vec2(0.f, direction.y * -1));
			position += glm::vec2(0, shortestYDist) * correctionAxis;
		}
	}
	else
	{
		if (shortestXDist < shortestYDist)
		{
			glm::vec2 correctionAxis = glm::normalize(glm::vec2(direction.x * -1, 0.f));
			position += glm::vec2(shortestXDist, 0) * correctionAxis;
		}
		else
		{
			glm::vec2 correctionAxis = glm::normalize(glm::vec2(0.f, direction.y * -1));
			position += glm::vec2(0, shortestYDist) * correctionAxis;
		}
	}
}

void Collider2D::ResolveAABBCircle(Collider2D* object, Collision data, ColliderType target)
{
	Collider2D* ball = nullptr;
	Collider2D* quad = nullptr;

	if (object->colliderType == Collider2D::COLLIDER_CIRCLE)
	{
		quad = this;
		ball = object;
	}
	else if (colliderType == Collider2D::COLLIDER_CIRCLE)
	{
		quad = object;
		ball = this;
	}

	if (ball)
	{

		// collision resolution
		Direction dir = std::get<1>(data);
		glm::vec2 diff_vector = std::get<2>(data);
		if (target == COLLIDER_CIRCLE)
		{
			if (dir == LEFT || dir == RIGHT) // horizontal collision
			{
				// relocate
				float penetration = ball->vec2Dimensions.x - std::abs(diff_vector.x);

				if (dir == LEFT)
					ball->position.x += penetration; // move ball to right
				else
					ball->position.x -= penetration; // move ball to left;
			}
			else // vertical collision
			{
				// relocate
				float penetration = ball->vec2Dimensions.x - std::abs(diff_vector.y);

				if (dir == UP)
					ball->position.y -= penetration; // move ball bback up
				else
					ball->position.y += penetration; // move ball back down

			}
		}
		else if (target == COLLIDER_QUAD)
		{
			if (dir == LEFT || dir == RIGHT) // horizontal collision
			{
				// relocate
				float penetration = ball->vec2Dimensions.x - std::abs(diff_vector.x);

				if (dir == LEFT)
					quad->position.x -= penetration; // move ball to right
				else
					quad->position.x += penetration; // move ball to left;
			}
			else // vertical collision
			{
				// relocate
				float penetration = ball->vec2Dimensions.x - std::abs(diff_vector.y);

				if (dir == UP)
					quad->position.y += penetration; // move ball bback up
				else
					quad->position.y -= penetration; // move ball back down

			}
		}
	}
}

void Collider2D::PreRender(void)
{
	// Use the shader defined for this class
	CShaderManager::GetInstance()->Use(sLineShaderName);
}

void Collider2D::Render(void)
{
	if (!bIsDisplayed)
		return;


	//Camera init
	glm::vec2 offset = glm::vec2(float(CSettings::GetInstance()->NUM_TILES_XAXIS / 2.f) - 0.5f, float(CSettings::GetInstance()->NUM_TILES_YAXIS / 2.f) - 0.5f);
	glm::vec2 cameraPos = Camera2D::GetInstance()->getCurrPos();

	glm::vec2 objCamPos = position - cameraPos + offset;

	glm::vec2 actualPos = CSettings::GetInstance()->ConvertIndexToUVSpace(objCamPos);

	float clampOffset = cSettings->ConvertIndexToUVSpace(CSettings::AXIS::x, 1, false) / 2;
	clampOffset = (clampOffset + 1);

	float clampX = 1.0f + clampOffset;
	float clampY = 1.0f + clampOffset;
	if (fabs(actualPos.x) < clampX && fabs(actualPos.y) < clampY)
	{
		transform = glm::mat4(1.f);
		transform = glm::translate(transform, glm::vec3(actualPos.x, actualPos.y, 0.f));
		transform = glm::scale(transform, glm::vec3(CSettings::GetInstance()->TILE_WIDTH, CSettings::GetInstance()->TILE_HEIGHT, 1.f));
		CShaderManager::GetInstance()->activeShader->setMat4("transform", transform);

		// render box
		glBindVertexArray(VAO);
		glDrawArrays(GL_LINE_LOOP, 0, 6);
	}
}

void Collider2D::PostRender(void)
{
}

bool Collider2D::GetbEnabled() const
{
	return bEnabled;
}

void Collider2D::SetbEnabled(bool bEnabled)
{
	this->bEnabled = bEnabled;
}

glm::vec2 Collider2D::GetPosition() const
{
	return position;
}

void Collider2D::SetPosition(glm::vec2 position)
{
	this->position = position;
}