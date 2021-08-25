#pragma once

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include <vector>
#include <math.h>

class Collider2D;
class CEntity2D;
class CMap2D;

class RayCast2D
{
	public:
		RayCast2D(void);
		~RayCast2D(void);

		//Initialisation
		void Init(CEntity2D* currentTarget , std::vector<CEntity2D*> entityArr = std::vector<CEntity2D*>());

		bool RayCheck(void); //True if raycast is successful and there's no object in the way

		void PreRender(void);
		void Render(void);
		void PostRender(void);

		void RenderRayCast(void);

	private:
		//Misc information
		glm::vec2 originPoint;
		glm::vec2 currentPoint;
		glm::vec2 targetPoint;

		CEntity2D* client;
		CEntity2D* castedEntity;

		//Collider2D
		Collider2D* collider2D;

		//Mapping
		std::vector<CEntity2D*> entityArr;
		CMap2D* cMap2D;

		//Rendering
		unsigned int VAO, VBO;
		glm::mat4 transform;
};

