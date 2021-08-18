/**
 Map2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Map2D.h"

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include Filesystem
#include "System\filesystem.h"
// Include ImageLoader
#include "System\ImageLoader.h"
#include "Primitives/MeshBuilder.h"

#include <iostream>
#include <vector>

using namespace std;

// For AStar PathFinding
using namespace std::placeholders;

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CMap2D::CMap2D(void)
	: uiCurLevel(0)
	, camera(NULL)
{
	arrMapSizes = nullptr;
	quadMesh = nullptr;
	uiNumLevels = 0;
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CMap2D::~CMap2D(void)
{
	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// Set this to NULL since it was created elsewhere, so we let it be deleted there.
	cSettings = NULL;
}

/**
@brief Init Initialise this instance
*/
bool CMap2D::Init(const unsigned int uiNumLevels,
	const unsigned int uiNumRows,
	const unsigned int uiNumCols)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	//Initialising arrays and stuff
	arrLevelLimit.clear();
	arrGrid.clear();
	arrObject.clear();
	for (unsigned i = 0; i < uiNumLevels; i++) {
		arrLevelLimit.push_back(glm::i32vec2());
		arrObject.push_back(std::vector<CObject2D*>());
		arrGrid.push_back(std::vector<std::vector<CObject2D*>>());
	}


	// Store the map sizes in cSettings
	uiCurLevel = 0;
	this->uiNumLevels = uiNumLevels;
	cSettings->NUM_TILES_XAXIS = uiNumCols;
	cSettings->NUM_TILES_YAXIS = uiNumRows;
	cSettings->UpdateSpecifications();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS: Create the Quad Mesh using the mesh builder
	quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	// Load and create textures
	// Load the ground texture

	/// <summary>
	/// 2 - 99 -> NO COLLISION
	///		2 - 10  -> Collectibles
	///		11 - 20 -> Interactables?
	///			11 - 20 -> Toggles
	///			16 - 20 -> In Range??
	///		21 - 30 -> AOE Effect
	/// 
	/// 100 - 200 -> COLLISION
	/// 300 - 400 -> ENEMY / ENTITY
	/// </summary>
	
	if (LoadTexture("Image/Ground/GroundTile.png", 100) == false)
	{
		std::cout << "Failed to load ground tile texture" << std::endl;
		return false;
	}
	if (LoadTexture("Image/Ground/GrassTile.png", 103) == false)
	{
		std::cout << "Failed to load GrassTile tile texture" << std::endl;
		return false;
	}
	if (LoadTexture("Image/Ground/LeftGrassTile.png", 104) == false)
	{
		std::cout << "Failed to load LeftGrassTile tile texture" << std::endl;
		return false;
	}
	if (LoadTexture("Image/Ground/RightGrassTile.png", 105) == false)
	{
		std::cout << "Failed to load RightGrassTile tile texture" << std::endl;
		return false;
	}


	if (LoadTexture("Image/Walls/wall_1.png", 101) == false)
	{
		std::cout << "Failed to load wall_1 tile texture" << std::endl;
		return false;
	}
	if (LoadTexture("Image/Walls/wall_2.png", 102) == false)
	{
		std::cout << "Failed to load wall_2 tile texture" << std::endl;
		return false;
	}
	
	if (LoadTexture("Image/Collectibles/shuriken.png", 2) == false)
	{
		std::cout << "Failed to load tree tile texture" << std::endl;
		return false;
	}

	if (LoadTexture("Image/Collectibles/Scene2D_Lives.tga", 10) == false)
	{
		std::cout << "Failed to load Scene2D_Lives tile texture" << std::endl;
		return false;
	}

	if (LoadTexture("Image/Switches/switch_1_off.png", 11) == false)
	{
		std::cout << "Failed to load switch_1_off tile texture" << std::endl;
		return false;
	}
	if (LoadTexture("Image/Switches/switch_1_on.png", 12) == false)
	{
		std::cout << "Failed to load switch_1_on tile texture" << std::endl;
		return false;
	}

	if (LoadTexture("Image/Switches/switch_2_off.png", 13) == false)
	{
		std::cout << "Failed to load switch_2_off tile texture" << std::endl;
		return false;
	}
	if (LoadTexture("Image/Switches/switch_2_on.png", 14) == false)
	{
		std::cout << "Failed to load switch_2_on tile texture" << std::endl;
		return false;
	}

	if (LoadTexture("Image/Portals/portal.png", 15) == false)
	{
		std::cout << "Failed to load portal15 tile texture" << std::endl;
		return false;
	}
	if (LoadTexture("Image/Portals/portal.png", 16) == false)
	{
		std::cout << "Failed to load portal16 tile texture" << std::endl;
		return false;
	}

	if (LoadTexture("Image/Portals/portalgate.png", 17) == false)
	{
		std::cout << "Failed to load portalgate tile texture" << std::endl;
		return false;
	}
	if (LoadTexture("Image/Portals/portalgate_off.png", 18) == false)
	{
		std::cout << "Failed to load portalgate_off tile texture" << std::endl;
		return false;
	}

	if (LoadTexture("Image/Scene2D_Spikes.tga", 20) == false)
	{
		std::cout << "Failed to load Scene2D_Spikes tile texture" << std::endl;
		return false;
	}

	if (LoadTexture("Image/altar.png", 21) == false)
	{
		std::cout << "Failed to load Scene2D_Spa tile texture" << std::endl;
		return false;
	}

	//Camera
	camera = Camera2D::GetInstance();

	return true;
}

/**
@brief Update Update this instance
*/
void CMap2D::Update(const double dElapsedTime)
{	
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CMap2D::PreRender(void)
{
	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render Render this instance
 */
void CMap2D::Render(void)
{
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	//Render(MY VERSION)

	//Camera init
	glm::vec2 offset = glm::vec2(float(cSettings->NUM_TILES_XAXIS / 2.f) - 0.5f, float(cSettings->NUM_TILES_YAXIS / 2.f) - 0.5f);
	glm::vec2 cameraPos = camera->getCurrPos();

	for (unsigned i = 0; i < arrObject[uiCurLevel].size(); i++) {
		const CObject2D* currObj = arrObject[uiCurLevel][i];
		glm::vec2 objCamPos = currObj->indexSpace - cameraPos + offset;

		glm::vec2 actualPos = cSettings->ConvertIndexToUVSpace(objCamPos);

		float clampX = 1.01f;
		float clampY = 1.01f;
		if (actualPos.x <= -clampX || actualPos.x >= clampX || actualPos.y <= -clampY || actualPos.y >= clampY)
			continue;

		transform = glm::mat4(1.f);
		transform = glm::translate(transform, glm::vec3(actualPos.x, actualPos.y, 0.f));
		/*transform = glm::translate(transform, glm::vec3(
			cSettings->ConvertIndexToUVSpace(cSettings->x, currObj.indexSpace.x, false, 0),
			cSettings->ConvertIndexToUVSpace(cSettings->y, currObj.indexSpace.y, false),
			0.f
		));*/

		// Update the shaders with the latest transform
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		RenderTile(currObj);
	}
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CMap2D::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);

	for (unsigned i = 0; i < arrObject[uiCurLevel].size(); i++)
	{
		CObject2D* currObj = arrObject[uiCurLevel][i];

		currObj->collider2D.PreRender();
		currObj->collider2D.Render();
		currObj->collider2D.PostRender();
	}
}

int CMap2D::GetLevelCol(void) {
	return arrLevelLimit[uiCurLevel].x;
}

int CMap2D::GetLevelRow(void) {
	return arrLevelLimit[uiCurLevel].y;
}

glm::i32vec2 CMap2D::GetLevelLimit(void) {
	return arrLevelLimit[uiCurLevel];
}

// Set the specifications of the map
void CMap2D::SetNumTiles(const CSettings::AXIS sAxis, const unsigned int uiValue)
{
	// Check if the value is valid
	if (uiValue <= 0)
	{
		cout << "CMap2D::SetNumTiles() : value must be more than 0" << endl;
		return;
	}

	if (sAxis == CSettings::x)
	{
		cSettings->NUM_TILES_XAXIS = uiValue;
		cSettings->UpdateSpecifications();
	}
	else if (sAxis == CSettings::y)
	{
		cSettings->NUM_TILES_YAXIS = uiValue;
		cSettings->UpdateSpecifications();
	}
	else if (sAxis == CSettings::z)
	{
		// Not used in here
	}
	else
	{
		cout << "Unknown axis" << endl;
	}
}

// Set the specifications of the map
void CMap2D::SetNumSteps(const CSettings::AXIS sAxis, const unsigned int uiValue)
{
	// Check if the value is valid
	if (uiValue <= 0)
	{
		cout << "CMap2D::SetNumSteps() : value must be more than 0" << endl;
		return;
	}

	if (sAxis == CSettings::x)
	{
		cSettings->NUM_STEPS_PER_TILE_XAXIS = (float)uiValue;
		cSettings->UpdateSpecifications();
	}
	else if (sAxis == CSettings::y)
	{
		cSettings->NUM_STEPS_PER_TILE_YAXIS = (float)uiValue;
		cSettings->UpdateSpecifications();
	}
	else if (sAxis == CSettings::z)
	{
		// Not used in here
	}
	else
	{
		cout << "Unknown axis" << endl;
	}
}

/**
 @brief Set the value at certain indices in the arrMapInfo
 @param iRow A const int variable containing the row index of the element to set to
 @param iCol A const int variable containing the column index of the element to set to
 @param iValue A const int variable containing the value to assign to this arrMapInfo
 */
void CMap2D::SetMapInfo(const unsigned int uiRow, const unsigned int uiCol, const int iValue, const bool bInvert)
{
	for (unsigned i = 0; i < arrObject[uiCurLevel].size(); i++) {
		CObject2D* obj = arrObject[uiCurLevel][i];

		if (obj->indexSpace.x == uiCol && obj->indexSpace.y == uiRow) {
			if (iValue == 0) {
				arrGrid[uiCurLevel][GetLevelRow() - obj->indexSpace.y - 1][obj->indexSpace.x] = nullptr;

				delete obj;
				arrObject[uiCurLevel][i] = nullptr;
				arrObject[uiCurLevel].erase(arrObject[uiCurLevel].begin() + i);


				return;
			}

			obj->value = iValue;
			return;
		}
	}

	if (iValue == 0)
		return;

	CObject2D* newObj = new CObject2D;
	newObj->setIndexSpace(glm::i32vec2(uiCol, uiRow));
	newObj->value = iValue;

	arrObject[uiCurLevel].push_back(newObj);
}

void CMap2D::ToggleMapInfo(const unsigned int uiRow, const unsigned int uiCol, const bool iValue, const bool bInvert)
{
	//Do nothing for now
}

std::vector<CObject2D*> CMap2D::GetObjectArr()
{
	return arrObject[uiCurLevel];
}

CObject2D* CMap2D::GetCObject(const unsigned int uiCol, const unsigned int uiRow, const bool bInvert)
{
	if(bInvert)
		return arrGrid[uiCurLevel][GetLevelRow() - uiRow - 1][uiCol];
	else
		return arrGrid[uiCurLevel][uiRow][uiCol];
}

/**
 @brief Get the value at certain indices in the arrMapInfo
 @param iRow A const int variable containing the row index of the element to get from
 @param iCol A const int variable containing the column index of the element to get from
 @param bInvert A const bool variable which indicates if the row information is inverted
 */
int CMap2D::GetMapInfo(const unsigned int uiRow, const unsigned int uiCol, const bool bInvert) const
{
	//Check if theres object on tile
	for (unsigned i = 0; i < arrObject[uiCurLevel].size(); i++) {
		const CObject2D* obj = arrObject[uiCurLevel][i];

		if (obj->indexSpace.x == uiCol && obj->indexSpace.y == uiRow)
			return obj->value;
	}

	//Return false if theres nothing on the tile
	return 0;
}

bool CMap2D::GetMapActive(const unsigned int uiRow, const unsigned int uiCol, const bool bInvert) const
{
	//Check if theres object on tile
	for (unsigned i = 0; i < arrObject[uiCurLevel].size(); i++) {
		const CObject2D* obj = arrObject[uiCurLevel][i];

		if (obj->indexSpace.x == uiCol && obj->indexSpace.y == uiRow)
			return true;
	}

	//Return false if theres nothing on the tile
	return false;
}


/**
 @brief Load a map
 */ 
bool CMap2D::LoadMap(string filename, const unsigned int uiCurLevel)
{
	doc = rapidcsv::Document(FileSystem::getPath(filename).c_str());

	// Check if the sizes of CSV data matches the declared arrMapInfo sizes
	/*if ((cSettings->NUM_TILES_XAXIS != (unsigned int)doc.GetColumnCount()) ||
		(cSettings->NUM_TILES_YAXIS != (unsigned int)doc.GetRowCount()))
	{
		cout << "Sizes of CSV map does not match declared arrMapInfo sizes." << endl;
		return false;
	}*/

	arrLevelLimit[uiCurLevel] = glm::i32vec2(doc.GetColumnCount(), doc.GetRowCount());

	// Read the rows and columns of CSV data into arrMapInfo
	for (unsigned int uiRow = 0; uiRow < (unsigned int)doc.GetRowCount(); uiRow++)
	{
		// Read a row from the CSV file
		std::vector<std::string> row = doc.GetRow<std::string>(uiRow);

		arrGrid[uiCurLevel].push_back(std::vector<CObject2D*>());
		
		// Load a particular CSV value into the arrMapInfo
		for (unsigned int uiCol = 0; uiCol < (unsigned int)doc.GetColumnCount(); ++uiCol)
		{
			//Init of objects values
			int currVal = (int)stoi(row[uiCol]);
			bool currCollide = false;

			CObject2D* currObj = new CObject2D();
			currObj->value = currVal;
			if (currVal >= 100)
				currCollide = true;
			currObj->collidable = currCollide;

			//Position of values
			glm::vec2 currIndex;
			currIndex.x = (float)uiCol;
			currIndex.y = (float)doc.GetRowCount() - (float)uiRow - 1.f;

			currObj->setIndexSpace(currIndex);

			//Collider2D initialisation
			currObj->collider2D.Init();
			currObj->collider2D.position = glm::vec3(
				currIndex.x  + 0.5f,
				currIndex.y  + 0.5f,
				0.f
			);

			if (currVal >= 100 && currVal < 300)
				currObj->collider2D.colliderEnabled = true;
			else
				currObj->collider2D.colliderEnabled = false;
			
			arrGrid[uiCurLevel][uiRow].push_back(nullptr);

			if (currVal > 0) {
				arrObject[uiCurLevel].push_back(currObj);

				//Add in new CObj pointer if available
				arrGrid[uiCurLevel][uiRow][uiCol] = currObj;
			}
		}
	}

	return true;
}

/**
 @brief Save the tilemap to a text file
 @param filename A string variable containing the name of the text file to save the map to
 */
bool CMap2D::SaveMap(string filename, const unsigned int uiCurLevel)
{
	// Update the rapidcsv::Document from arrMapInfo

	//Reset the map
	for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
	{
		for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; uiCol++)
		{
			doc.SetCell(uiCol, uiRow, 0);
		}
		//cout << endl; //Add back if i mess up somewhere future jevon you know what i mean dont cb
	}

	for (unsigned i = 0; i < arrObject[uiCurLevel].size(); i++) {
		CObject2D* obj = arrObject[uiCurLevel][i];

		doc.SetCell(obj->indexSpace.x, cSettings->NUM_TILES_YAXIS - 1 - obj->indexSpace.y, obj->value);
	}

	// Save the rapidcsv::Document to a file
	doc.Save(FileSystem::getPath(filename).c_str());

	return true;
}

/**
@brief Find the indices of a certain value in arrMapInfo
@param iValue A const int variable containing the row index of the found element
@param iRow A const int variable containing the row index of the found element
@param iCol A const int variable containing the column index of the found element
@param bInvert A const bool variable which indicates if the row information is inverted
*/
bool CMap2D::FindValue(const int iValue, unsigned int& uirRow, unsigned int& uirCol)
{
	for (unsigned i = 0; i < arrObject[uiCurLevel].size(); i++) {
		CObject2D* obj = arrObject[uiCurLevel][i];

		if (obj->value == iValue) {
			uirCol = obj->indexSpace.x;
			uirRow = obj->indexSpace.y; //For now keep the same

			return true;
		}
	}

	return false;
}

/**
 @brief Set current level
 */
void CMap2D::SetCurrentLevel(unsigned int uiCurLevel)
{
	if (uiCurLevel < uiNumLevels)
	{
		this->uiCurLevel = uiCurLevel;
	}
}
/**
 @brief Get current level
 */
unsigned int CMap2D::GetCurrentLevel(void) const
{
	return uiCurLevel;
}

/**
 @brief Load a texture, assign it a code and store it in MapOfTextureIDs.
 @param filename A const char* variable which contains the file name of the texture
 @param iTextureCode A const int variable which is the texture code.
 */
bool CMap2D::LoadTexture(const char* filename, const int iTextureCode)
{
	// Variables used in loading the texture
	int width, height, nrChannels;
	unsigned int textureID;

	// texture 1
	// ---------
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	CImageLoader* cImageLoader = CImageLoader::GetInstance();
	unsigned char *data = cImageLoader->Load(filename, width, height, nrChannels, true);
	if (data)
	{
		if (nrChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if (nrChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		// Generate mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);

		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(iTextureCode, textureID));
	}
	else
	{
		return false;
	}
	// Free up the memory of the file data read in
	free(data);

	return true;
}

void CMap2D::RenderTile(const CObject2D* obj) {
	glBindTexture(GL_TEXTURE_2D, MapOfTextureIDs.at(obj->value));

	glBindVertexArray(VAO);
	//CS: Render the tile
	quadMesh->Render();
	glBindVertexArray(0);
}
