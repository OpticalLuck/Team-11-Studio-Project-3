#pragma once

// Include shader
#include "RenderControl\shader.h"

//CEntity2D
#include "../Library/Source/Primitives/Entity2D.h"

// Include the Map2D as we will use it to check the player's movements and actions
class CMap2D;

// Include Settings
#include "GameControl\Settings.h"

//Camera
#include "Camera2D.h"

class CBoss2D : public CEntity2D
{
	public :
		CBoss2D(void);
		virtual ~CBoss2D(void);

		bool Init(void);

		// Update
		void Update(const double dElapsedTime);

		// PreRender
		void PreRender(void);

		// Render
		void Render(void);

		// PostRender
		void PostRender(void);

	protected:
		bool isNear;
		int health;

		float bulletAng; //Angle of which where the bullet will come from

		//Rendering
		//Mesh
		CMesh* quadMesh;

		// Current color
		glm::vec4 currentColor;
		bool bIsActive;

		//Handlers
		CMap2D* cMap2D;
		Camera2D* camera;

		//Functions
		// Load a texture
		bool LoadTexture(const char* filename, GLuint& iTextureID);
};

