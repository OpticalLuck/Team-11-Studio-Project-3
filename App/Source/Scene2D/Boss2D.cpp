#include "Boss2D.h"

// Include Map
#include "Map2D.h"
// Include Mesh Builder
#include "Primitives/MeshBuilder.h"
// Include ImageLoader
#include "System\ImageLoader.h"
//Include Math.h
#include "Math/MyMath.h"
//Include shader manager
#include "RenderControl/ShaderManager.h"
//Include CEntityManager
#include "EntityManager.h"

CBoss2D::CBoss2D(void) {
	bIsActive = false;
	bulletAng = 0;
	cMap2D = nullptr;
	currentColor = glm::vec4();
	health = 0;
	currTarget = nullptr;
	quadMesh = nullptr;
	camera = nullptr;

	arrFSM = nullptr;
	arrAtkDuration = arrPauseDuration = nullptr;
	cEntityManager = nullptr;

	fsmIndex = roundIndex = 0;
}

CBoss2D::~CBoss2D(void) {
	//Delete fsm array
	delete[] arrFSM;
	arrFSM = nullptr;

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

bool CBoss2D::Init(void) {
	cSettings = CSettings::GetInstance();
	cMap2D = CMap2D::GetInstance();
	camera = Camera2D::GetInstance();
	cEntityManager = CEntityManager::GetInstance();

	arrPlayer = cEntityManager->GetAllPlayers();

	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(305, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the boss, so quit this game

	// Erase the value of the boss in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	vTransform = glm::i32vec2(uiCol, uiRow);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS: Create the Quad Mesh using the mesh builder
	if (!quadMesh)
		quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	// Load the enemy2D texture
	if (LoadTexture("Image/Scene2D_EnemyTile.tga", iTextureID) == false)
	{
		std::cout << "Failed to load enemy2D tile texture" << std::endl;
		return false;
	}

	//CS: Init the color to white
	currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;

	collider2D->Init();
	//Update collider to Boss position
	collider2D->SetPosition(glm::vec3(vTransform, 0.f));

	//Initialisation of variables
	bulletAng = (float)Math::RandIntMinMax(0, 359);
	health = 100;
	currTarget = nullptr;

	//Array of vectors
	fsmIndex = 0;
	roundIndex = 0;

	if (!arrFSM) {
		arrFSM = new std::vector<ATK>[5];
		arrFSM[roundIndex].push_back((ATK)RandomiseAttack());
	}

	if (!arrAtkDuration) {
		arrAtkDuration = new std::vector<int>[5];
		arrAtkDuration[roundIndex].push_back(RandomiseTimer(true));
	}

	if (!arrPauseDuration) {
		arrPauseDuration = new std::vector<int>[5];
		arrPauseDuration[roundIndex].push_back(RandomiseTimer(false));
	}

	return true;
}

CBoss2D::ATK CBoss2D::RandomiseAttack(void) {
	return ATK(Math::RandIntMinMax(0, (int)ATK::A_TOTAL - 1));
}

int CBoss2D::RandomiseTimer(bool atk) {
	float value = 0; //In terms of seconds first

	if (atk)
		value = Math::RandFloatMinMax(3, 7.5f);
	else
		value = Math::RandFloatMinMax(1.5f, 4);

	return int(value * (float)cSettings->FPS); //Convert to in terms of frames
}

bool CBoss2D::LoadTexture(const char* filename, GLuint& iTextureID) {
	// Variables used in loading the texture
	int width, height, nrChannels;

	// texture 1
	// ---------
	glGenTextures(1, &iTextureID);
	glBindTexture(GL_TEXTURE_2D, iTextureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	CImageLoader* cImageLoader = CImageLoader::GetInstance();
	unsigned char* data = cImageLoader->Load(filename, width, height, nrChannels, true);
	if (data)
	{
		if (nrChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if (nrChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		// Generate mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		return false;
	}
	// Free up the memory of the file data read in
	free(data);

	return true;
}

void CBoss2D::Update(const double dElapsedTime) {
	if (!bIsActive)
		return; //Return if boss is not active

	if (currTarget) {
		UpdateAttack((float)dElapsedTime);
	}
	else {
		//Check if player is near and enable boss fight
		currTarget = GetNearestTarget();
	}
}

void CBoss2D::UpdateAttack(float dElapsedTime) {

}

void CBoss2D::PreRender(void) {
	if (!bIsActive)
		return;

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

void CBoss2D::Render(void) {
	if (!bIsActive)
		return;

	glBindVertexArray(VAO);
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	unsigned int colorLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "runtime_color");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	//Get camera transforms and use them instead
	glm::vec2 offset = glm::vec2((cSettings->NUM_TILES_XAXIS / 2.f) - 0.5f, (cSettings->NUM_TILES_YAXIS / 2.f) - 0.5f);
	glm::vec2 cameraPos = camera->getCurrPos();

	glm::vec2 IndexPos = vTransform;

	glm::vec2 actualPos = IndexPos - cameraPos + offset;
	actualPos = cSettings->ConvertIndexToUVSpace(actualPos);

	float clampOffset = cSettings->ConvertIndexToUVSpace(CSettings::AXIS::x, 1, false) / 2;
	clampOffset = (clampOffset + 1);

	float clampX = 1.0f + clampOffset;
	float clampY = 1.0f + clampOffset;
	if (actualPos.x <= -clampX || actualPos.x >= clampX || actualPos.y <= -clampY || actualPos.y >= clampY)
		return; //Exit code if enemy is too far to be rendered

	transform = glm::translate(transform, glm::vec3(actualPos.x, actualPos.y, 0.f));

	/*if (facing == LEFT) //ROTATION CODE: UNCOMMENT IF NEEDED
		transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));*/

	// Update the shaders with the latest transform
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform4fv(colorLoc, 1, glm::value_ptr(currentColor));

	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);

	// Render the tile
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	quadMesh->Render();

	glBindVertexArray(0);
}

void CBoss2D::PostRender(void) {
	if (!bIsActive)
		return;

	// Disable blending
	glDisable(GL_BLEND);
}