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

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CMap2D::CMap2D(void)
	: uiCurLevel(0)
	, camera(NULL)
{
	uiNumLevels = 0;
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CMap2D::~CMap2D(void)
{
	for (unsigned i = 0; i < arrObject.size(); i++) {
		for (unsigned x = 0; x < arrObject[i].size(); x++) {
			delete arrObject[i][x];
			arrObject[i][x] = nullptr;
		}
	} 
	arrObject.clear();
	arrGrid.clear();

	//Deletion of background
	for (unsigned i = 0; i < arrBackground.size(); i++) {
		if (arrBackground[i]) {
			delete arrBackground[i];
			arrBackground[i] = nullptr;
		}
	}
	arrBackground.clear();
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

	// Get the handler to the CTextureManager instance
	cTextureManager = CTextureManager::GetInstance();

	//Initialising objects
	arrObject.clear();
	for (unsigned i = 0; i < uiNumLevels; i++)
		arrObject.push_back(std::vector<CObject2D*>());

	//Initialising limits of map
	arrLevelLimit.clear();
	arrGrid.clear();
	arrObject.clear();
	arrBackground.clear();
	arrAllowanceScale.clear();

	for (unsigned i = 0; i < uiNumLevels; i++) {
		arrLevelLimit.push_back(glm::i32vec2());
		arrObject.push_back(std::vector<CObject2D*>());
		arrGrid.push_back(std::vector<std::vector<CObject2D*>>());
		arrBackground.push_back(nullptr);
		arrAllowanceScale.push_back(glm::vec2());
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

	//Camera
	camera = Camera2D::GetInstance();

	return true;
}

/**
@brief Update Update this instance
*/
void CMap2D::Update(const double dElapsedTime)
{
	for (auto object : arrObject[uiCurLevel])
	{
		object->Update(dElapsedTime);
	}
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
	glm::vec2 offset = glm::vec2(float(cSettings->NUM_TILES_XAXIS / 2.f), float(cSettings->NUM_TILES_YAXIS / 2.f));
	glm::vec2 cameraPos = camera->getCurrPos();

	for (unsigned i = 0; i < arrObject[uiCurLevel].size(); i++) {
		CObject2D* currObj = arrObject[uiCurLevel][i];
		glm::vec2 objCamPos = currObj->vTransform - cameraPos + offset;

		glm::vec2 actualPos = cSettings->ConvertIndexToUVSpace(objCamPos);

		float clampOffset = cSettings->ConvertIndexToUVSpace(CSettings::AXIS::x, 1, false) / 2;

		float clampX = 2.0f + clampOffset;
		float clampY = 2.0f + clampOffset;

		if (actualPos.x <= -clampX || actualPos.x >= clampX || actualPos.y <= -clampY || actualPos.y >= clampY)
			continue;

		transform = glm::mat4(1.f);
		transform = glm::translate(transform, glm::vec3(actualPos.x, actualPos.y, 0.f));

		// Update the shaders with the latest transform
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		RenderTile(*currObj);
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

		currObj->GetCollider()->PreRender();
		currObj->GetCollider()->Render();
		currObj->GetCollider()->PostRender();
	}
}

int CMap2D::GetLevelCol(void) {
	return arrLevelLimit[uiCurLevel].x;
}

int CMap2D::GetLevelRow(void) {
	return arrLevelLimit[uiCurLevel].y;
}

glm::i32vec2 CMap2D::GetLevelLimit(void) {
	return glm::vec2(arrLevelLimit[uiCurLevel].x, arrLevelLimit[uiCurLevel].y);
}

/**
 @brief Set the value at certain indices in the arrMapInfo
 @param iRow A const int variable containing the row index of the element to set to
 @param iCol A const int variable containing the column index of the element to set to
 @param iValue A const int variable containing the value to assign to this arrMapInfo
 */
void CMap2D::SetMapInfo(unsigned int uiRow, unsigned int uiCol, const int iValue, const bool bInvert)
{
	if (bInvert)
	{
		uiRow = GetLevelRow() - uiRow - 1;
	}

	//Check if its not a nullptr
	if (!arrGrid[uiCurLevel][uiRow][uiCol]) 
	{
		CObject2D* currObj = objFactory.CreateObject(iValue);
		currObj->SetCurrentIndex(glm::i32vec2(uiCol, uiRow));
		currObj->vTransform = glm::i32vec2(uiCol, GetLevelRow() - uiRow - 1);
		currObj->Init();

		arrObject[uiCurLevel].push_back(currObj);

		arrGrid[uiCurLevel][uiRow][uiCol] = currObj;
	}
	else
	{
		for (unsigned i = 0; i < arrObject[uiCurLevel].size(); i++)
		{
			CObject2D* currObj = arrObject[uiCurLevel][i];
			if (currObj->GetCurrentIndex() == glm::i32vec2(uiCol, uiRow))
			{
				//Delete occupying object in arrObj
				delete currObj;
				arrObject[uiCurLevel][i] = nullptr;
				arrObject[uiCurLevel].erase(arrObject[uiCurLevel].begin() + i);

				if (iValue > 0)
				{
					//Pushes in new Object
					currObj = objFactory.CreateObject(iValue);
					currObj->SetCurrentIndex(glm::i32vec2(uiCol, uiRow));
					currObj->vTransform = glm::i32vec2(uiCol, GetLevelRow() - uiRow - 1);
					currObj->Init();
					arrObject[uiCurLevel].push_back(currObj);
				
					arrGrid[uiCurLevel][uiRow][uiCol] = currObj;
				}
				else
				{
					arrGrid[uiCurLevel][uiRow][uiCol] = nullptr;
				}
			}
		}
	}
}

bool CMap2D::InsertMapInfo(unsigned int uiRow, unsigned int uiCol, const int iValue, const bool bInvert)
{
	if (bInvert)
	{
		uiRow = GetLevelRow() - uiRow - 1;
	}

	//Check if its not a nullptr
	if (!arrGrid[uiCurLevel][uiRow][uiCol])
	{
		CObject2D* currObj = objFactory.CreateObject(iValue);
		currObj->SetCurrentIndex(glm::i32vec2(uiCol, uiRow));
		currObj->vTransform = glm::i32vec2(uiCol, GetLevelRow() - uiRow - 1);
		currObj->Init();

		arrObject[uiCurLevel].push_back(currObj);

		arrGrid[uiCurLevel][uiRow][uiCol] = currObj;
	}
	else
	{
		cout << "Something is in Current TileGrid" << endl;
		return false;
	}
	return true;
}

void CMap2D::UpdateGridInfo(const unsigned int uiRow, const unsigned uiCol, CObject2D* target, const bool bInvert)
{
	// Used for checking current spot
	CObject2D* obj = nullptr;

	// Used to check new spot
	CObject2D* newSpot = nullptr;
	if (bInvert)
	{
		obj = arrGrid[uiCurLevel][GetLevelRow() - target->GetCurrentIndex().y - 1][target->GetCurrentIndex().x];
		newSpot =  arrGrid[uiCurLevel][GetLevelRow() - uiRow - 1][uiCol];
	}
	else
	{
		if (uiRow < GetLevelRow() && uiCol < GetLevelCol() && uiRow >= 0 && uiCol >= 0)
		{
			obj = arrGrid[uiCurLevel][target->GetCurrentIndex().y][target->GetCurrentIndex().x];
			newSpot = arrGrid[uiCurLevel][uiRow][uiCol];
		}
		else
		{
			cout << "Object went out of map range - Deleting" << endl;

			//Get Object in arrObject list
			for (unsigned i = 0; i < arrObject[uiCurLevel].size(); i++)
			{
				obj = arrObject[uiCurLevel][i];
				if (obj == arrGrid[uiCurLevel][target->GetCurrentIndex().y][target->GetCurrentIndex().x])
				{
					arrGrid[uiCurLevel][target->GetCurrentIndex().y][target->GetCurrentIndex().x] = nullptr;
					//Delete occupying object in arrObj
					delete obj;
					arrObject[uiCurLevel][i] = nullptr;
					arrObject[uiCurLevel].erase(arrObject[uiCurLevel].begin() + i);

					obj = nullptr;
					break;
				}
			}
		}
	}

	if (newSpot == nullptr)
	{
		if (obj == target)
		{
			if (bInvert)
			{
				arrGrid[uiCurLevel][target->GetCurrentIndex().y][target->GetCurrentIndex().x] = nullptr;
				arrGrid[uiCurLevel][uiRow][uiCol] = target;
			}
			else
			{
				arrGrid[uiCurLevel][target->GetCurrentIndex().y][target->GetCurrentIndex().x] = nullptr;
				arrGrid[uiCurLevel][uiRow][uiCol] = target;
			}

			target->SetCurrentIndex(glm::i32vec2(uiCol, uiRow));
		}
	}
	else
	{
		cout << "SOMETHING IN GRID SPACE" << endl;
	}
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
		CObject2D* obj = arrObject[uiCurLevel][i];

		if (obj->vTransform.x == uiCol && obj->vTransform.y == uiRow)
			return obj->Getvalue();
	}

	//Return false if theres nothing on the tile
	return 0;
}


/**
 @brief Load a map
 */
bool CMap2D::LoadMap(string filename, const unsigned int uiCurLevel)
{
	doc = rapidcsv::Document(FileSystem::getPath(filename).c_str());

	// Check if the sizes of CSV data matches the declared arrMapInfo sizes
	arrLevelLimit[uiCurLevel] = glm::i32vec2(doc.GetColumnCount() - 1, doc.GetRowCount());

	// Read the rows and columns of CSV data into arrMapInfo
	for (unsigned int uiRow = 0; uiRow < (unsigned int)doc.GetRowCount(); uiRow++)
	{
		// Read a row from the CSV file
		std::vector<std::string> row = doc.GetRow<std::string>(uiRow);

		//If iterating through first row, create BackgroundEntity if available
		if (uiRow == 2) { //3rd row
			//Creation of initialisation values
			std::string dir = doc.GetRow<std::string>(0)[doc.GetColumnCount() - 1];
			stringstream scaleTxt(doc.GetRow<std::string>(1)[doc.GetColumnCount() - 1]);
			stringstream allowanceScaleTxt(doc.GetRow<std::string>(2)[doc.GetColumnCount() - 1]);

			std::vector<float> scaleFloat;
			std::vector<float> allowanceScaleFloat;

			//Conversions from texts to floats :: 0 IS X AXIS AND 1 IS Y AXIS
			while (scaleTxt.good()) { 
				std::string substr;
				getline(scaleTxt, substr, ',');
				scaleFloat.push_back(stof(substr));
			}

			while (allowanceScaleTxt.good()) {
				std::string substr;
				getline(allowanceScaleTxt, substr, ',');
				allowanceScaleFloat.push_back(stof(substr));
			}

			arrAllowanceScale[uiCurLevel] = glm::vec2(allowanceScaleFloat[0], allowanceScaleFloat[1]);

			CBackgroundEntity* bgEntity = new CBackgroundEntity(dir);
			bgEntity->SetShader("2DShader");

			if (!bgEntity->Init(scaleFloat[0],scaleFloat[1]))  //If initialisation fails, delete value
				delete bgEntity;
			else
				arrBackground[uiCurLevel] = bgEntity; //Else put background into array

			//Cout for debugging purposes
			std::cout << "Background settings for level " << uiCurLevel + 1 << ": \n";
			std::cout << "Scale: " << scaleFloat[0] << ", " << scaleFloat[1] << std::endl;
			std::cout << "Allowance Scale: " << allowanceScaleFloat[0] << ", " << allowanceScaleFloat[1] << std::endl;
			if (arrBackground[uiCurLevel])
				std::cout << "Background loaded!\n";
			else
				std::cout << "Background is disabled\n";
		}

		arrGrid[uiCurLevel].push_back(std::vector<CObject2D*>());

		// Load a particular CSV value into the arrMapInfo
		for (unsigned int uiCol = 0; uiCol < (unsigned int)doc.GetColumnCount() - 1; ++uiCol)
		{
			//Init of objects values
			int currVal = (int)stoi(row[uiCol]);

			arrGrid[uiCurLevel][uiRow].push_back(nullptr);

			if (currVal > 0) {
				CObject2D* currObj = objFactory.CreateObject(currVal);
				currObj->SetValue(currVal);
				currObj->SetCurrentIndex(glm::i32vec2(uiCol, uiRow));

				//Position of values
				glm::vec2 currIndex;
				currIndex.x = (float)uiCol;
				currIndex.y = (float)doc.GetRowCount() - (float)uiRow - 1.f;
				currObj->vTransform = currIndex;

				currObj->Init();

				arrObject[uiCurLevel].push_back(currObj);
				//Add in new CObj pointer if available
				arrGrid[uiCurLevel][uiRow][uiCol] = currObj;
			}
		}
	}

	return true;
}

void CMap2D::RenderBackground(void) {
	if (arrBackground[uiCurLevel]) {
		//Parallax background math
		glm::vec2 allowance = glm::vec2((arrBackground[uiCurLevel]->scaleX - 2), (arrBackground[uiCurLevel]->scaleY - 2)); //Get allowance for offsetting

		//Scaling of allowance. 0.f to 1.f
		allowance.x *= arrAllowanceScale[uiCurLevel].x;
		allowance.y *= arrAllowanceScale[uiCurLevel].y;

		//Get beginning and end (In terms of how far the camera can go
		glm::vec2 beginning = glm::vec2((float(cSettings->NUM_TILES_XAXIS) / 2.f) - 1.f, (float(cSettings->NUM_TILES_YAXIS) / 2.f) - 1.f);
		glm::vec2 end = (glm::vec2)arrLevelLimit[uiCurLevel] - beginning;

		//Scaling of position
		glm::vec2 total = end - beginning;
		glm::vec2 curr = camera->getCurrPos() - beginning;

		float uvSpaceX = (curr.x / total.x) * allowance.x;
		float uvSpaceY = (curr.y / total.y) * allowance.y;

		uvSpaceX -= allowance.x / 2.f;
		uvSpaceY -= allowance.y / 2.f;

		glm::vec2 uvSpace = -glm::vec2(uvSpaceX, uvSpaceY);

		arrBackground[uiCurLevel]->vTransform = uvSpace;

		//Rendering
		arrBackground[uiCurLevel]->PreRender();
		arrBackground[uiCurLevel]->Render();
		arrBackground[uiCurLevel]->PostRender();
	}
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
		doc.SetCell((int)obj->vTransform.x, int(cSettings->NUM_TILES_YAXIS - 1 - obj->vTransform.y), obj->Getvalue());
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
		if (obj->Getvalue() == iValue) {
			uirCol = obj->vTransform.x;
			uirRow = obj->vTransform.y; //For now keep the same

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


void CMap2D::RenderTile(const CObject2D obj) {
	glBindTexture(GL_TEXTURE_2D, cTextureManager->MapOfTextureIDs.at(obj.Getvalue()));

	glBindVertexArray(VAO);
	//CS: Render the tile
	quadMesh->Render();
	glBindVertexArray(0);
}
