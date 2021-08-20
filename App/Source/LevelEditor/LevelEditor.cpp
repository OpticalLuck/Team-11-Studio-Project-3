#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include "LevelEditor.h"

#include <iostream>
#include <experimental/filesystem>

// Include Filesystem
#include "System\filesystem.h"

#include "System/Debug.h"
#include <fstream>

namespace fs = std::experimental::filesystem;

CLevelEditor::CLevelEditor()
    : iWorldWidth(32)
    , iWorldHeight(24)
    , quadMesh(NULL)
    , cSettings(NULL)
    , camera(NULL)
    , cTextureManager(NULL)
    , vAllowanceScale(1.f)
    , vUVCoords(2.f)
    , cBackgroundEntity(NULL)
{
}

CLevelEditor::~CLevelEditor()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    if (cSettings)
        cSettings = NULL;

    if (camera)
        camera = NULL;

    if (cTextureManager)
        cTextureManager = NULL;
}



void CLevelEditor::ResetParams()
{
    m_CurrentLevel.clear();
    vAllowanceScale = glm::vec2(1.f);
    vUVCoords = glm::vec2(2.f);
    backgroundPath = "";
    if (cBackgroundEntity)
        delete cBackgroundEntity;

    cBackgroundEntity = nullptr;
}

/**
@brief Initialise this instance
*/
void CLevelEditor::Init()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    cSettings = CSettings::GetInstance();
    camera = Camera2D::GetInstance();
    cTextureManager = CTextureManager::GetInstance();

    quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

    levelFolderPath = "Maps/";
    LoadExistingLevels();
}

void CLevelEditor::PreRender()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);

    // Activate the shader
    CShaderManager::GetInstance()->Use(sShaderName);
}

void CLevelEditor::Render()
{	
    // get matrix's uniform location and set matrix
    unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    //Render(MY VERSION)

    //Camera init
    glm::vec2 offset = glm::vec2(float(cSettings->NUM_TILES_XAXIS / 2.f), float(cSettings->NUM_TILES_YAXIS / 2.f));
    glm::vec2 cameraPos = camera->getCurrPos();

    for (unsigned int uiRow = 0; uiRow < iWorldHeight; ++uiRow) 
    {
        for (unsigned int uiCol = 0; uiCol < iWorldWidth; ++uiCol)
        {

            glm::vec2 objCamPos = glm::vec2(uiCol, uiRow) - cameraPos + offset;
            glm::vec2 actualPos = cSettings->ConvertIndexToUVSpace(objCamPos) * camera->getZoom();

            float clampX = 1.05f;
            float clampY = 1.05f;
            if (actualPos.x <= -clampX || actualPos.x >= clampX || actualPos.y <= -clampY || actualPos.y >= clampY)
                continue;

            transform = glm::mat4(1.f);
            transform = glm::translate(transform, glm::vec3(actualPos.x, actualPos.y, 0.f));
            transform = glm::scale(transform, glm::vec3(camera->getZoom()));

            // Update the shaders with the latest transform
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

            if (m_CurrentLevel[uiRow][uiCol].iTileID > 1 && m_CurrentLevel[uiRow][uiCol].iTileID != 305)
            {
                glBindTexture(GL_TEXTURE_2D, cTextureManager->MapOfTextureIDs.at(m_CurrentLevel[uiRow][uiCol].iTileID));

                glBindVertexArray(VAO);
                //CS: Render the tile
                quadMesh->Render();
                glBindVertexArray(0);
            }
        }
    }
}

void CLevelEditor::PostRender()
{	
    // Disable blending
    glDisable(GL_BLEND);
}

/**
@brief Creates a new level in m_CurrentLevel
*/
void CLevelEditor::CreateLevel(std::string levelName, unsigned int iWorldWidth, unsigned int iWorldHeight)
{
    // Reload the level data
    ResetParams();

    this->iWorldWidth = iWorldWidth;
    this->iWorldHeight = iWorldHeight;

    // Create a Dynamic Array with World Width and Height
    for (int iRow = 0; iRow < iWorldHeight; ++iRow)
    {
        m_CurrentLevel.push_back(std::vector<sCell>());
        for (int iCol = 0; iCol < iWorldWidth; ++iCol)
        {
            sCell cell;
            m_CurrentLevel[iRow].push_back(cell);
        }
    }

    currentLevel.LevelName = levelName;
    currentLevel.LevelPath = levelFolderPath + levelName + ".csv";

    DEBUG_MSG("Created New Level: " << levelName);
}

/**
@brief Loads an existing CSV into the level editor
*/
bool CLevelEditor::LoadLevel(const char* filePath)
{
    // Reload the level data
    ResetParams();

    // Load the Level CSV
    doc = rapidcsv::Document(FileSystem::getPath(filePath).c_str());

    // Set World Width and Height
    iWorldWidth = doc.GetColumnCount() - 1;
    iWorldHeight = doc.GetRowCount();

    fs::path p = filePath;
    currentLevel.LevelName = p.filename().generic_string();
    currentLevel.LevelName = currentLevel.LevelName.substr(0, currentLevel.LevelName.size() - 4); // remove ".csv"
    currentLevel.LevelPath = filePath;

    // Iterate through the Level Editor Map and set the values of the corresponding indexes
    for (int iRow = 0; iRow < iWorldHeight; ++iRow)
    {
        if (iRow == 2)
        {
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

            if (allowanceScaleFloat.size() == 2)
            {
                vAllowanceScale = glm::vec2(allowanceScaleFloat[0], allowanceScaleFloat[1]);
                vUVCoords = glm::vec2(scaleFloat[0], scaleFloat[1]);
                backgroundPath = dir;

                CBackgroundEntity* bgEntity = new CBackgroundEntity(dir);
                bgEntity->SetShader("2DShader");

                if (!bgEntity->Init(scaleFloat[0], scaleFloat[1]))  //If initialisation fails, delete value
                    delete bgEntity;
                else
                    cBackgroundEntity = bgEntity; //Else put background into array
            }

            //Cout for debugging purposes
            if (cBackgroundEntity)
                DEBUG_MSG("Background is loaded");
            else
                DEBUG_MSG("Background is disabled");
        }

        m_CurrentLevel.push_back(std::vector<sCell>());
        std::vector<std::string> row = doc.GetRow<std::string>(iRow);
        for (int iCol = 0; iCol < iWorldWidth; ++iCol)
        {
            sCell cell;
            m_CurrentLevel[iRow].push_back(cell);

            int currVal = (int)stoi(row[iCol]);
            m_CurrentLevel[iRow][iCol].iTileID = currVal;
        }
    }

    std::reverse(m_CurrentLevel.begin(), m_CurrentLevel.end());

    DEBUG_MSG("Opened " << filePath);
    return true;
}

/**
@brief Loads the level by name
*/
bool CLevelEditor::LoadLevelByName(std::string levelName)
{
    return LoadLevel((levelFolderPath + levelName + ".csv").c_str());
}


void CLevelEditor::SetShader(const std::string& _name)
{
    this->sShaderName = _name;
}

/**
@brief Returns the vector of Levels
*/
std::vector<Level> CLevelEditor::GetLevels(void)
{
    return m_Levels;
}

Level CLevelEditor::GetCurrentLevelData(void)
{
    return currentLevel;
}

std::vector<std::vector<sCell>> CLevelEditor::GetCurrentLevel(void)
{
    return m_CurrentLevel;
}

void CLevelEditor::SetCurrentLevel(std::vector<std::vector<sCell>> levelArr)
{
    m_CurrentLevel = levelArr;
}

/**
@brief Loads existing CSV files into the m_Levels vector
*/
void CLevelEditor::LoadExistingLevels(void)
{
    if (m_Levels.size() != 0)
        m_Levels.clear();

    for (const auto& file : fs::directory_iterator(levelFolderPath))
    {
        Level level;
        level.LevelName = file.path().filename().generic_string();
        level.LevelPath = file.path().generic_string();
        m_Levels.push_back(level);

        DEBUG_MSG("Loading: " << file.path());
        DEBUG_MSG("Loaded " << file.path().filename());
    }
}

/**
@brief Checks if the level already exists
*/
bool CLevelEditor::LevelExists(std::string levelName)
{
    for (const auto& file : fs::directory_iterator(levelFolderPath))
        if (levelName + ".csv" == file.path().filename().generic_string())
            return true;

    return false;
}

/**
@brief Saves the map into the csv file
*/
bool CLevelEditor::SaveMap()
{
    // Check if the level already exists
    if (!LevelExists(currentLevel.LevelName))
    {
        std::ofstream file;
        file.open(currentLevel.LevelPath, std::ios::out | std::ios::app);

        if (!file)
            return false;
    }         

    doc = rapidcsv::Document(FileSystem::getPath(currentLevel.LevelPath).c_str());
    for (unsigned int uiRow = 0; uiRow < iWorldHeight; uiRow++)
    {
        for (unsigned int uiCol = 0; uiCol < iWorldWidth; uiCol++)
        {
            doc.SetCell(uiCol, iWorldHeight - uiRow - 1, m_CurrentLevel[uiRow][uiCol].iTileID);
        }
    }

    std::string UVCoord = std::to_string(vUVCoords.x) + "," + std::to_string(vUVCoords.y);
    std::string allowance = std::to_string(vAllowanceScale.x) + "," + std::to_string(vAllowanceScale.y);

    doc.SetCell(iWorldWidth, 0, backgroundPath);
    doc.SetCell(iWorldWidth, 1, UVCoord);
    doc.SetCell(iWorldWidth, 2, allowance);

    // Save the rapidcsv::Document to a file
    doc.Save(FileSystem::getPath(currentLevel.LevelPath).c_str());
    DEBUG_MSG("Saved " << currentLevel.LevelName);
    return true;
}

bool CLevelEditor::IncreaseXSize(void)
{
    ++iWorldWidth;
 
    sCell cell;
    for (int i = 0; i < iWorldHeight; ++i)
    {
        m_CurrentLevel[i].push_back(cell);
    }
    return true;
}

bool CLevelEditor::DecreaseYSize(void)
{
    if (iWorldHeight == 24)
        return false;

    for (int i = 0; i < iWorldWidth; ++i)
    {
        if (m_CurrentLevel.back()[i].iTileID > 0)
            return false;
    }

    --iWorldHeight;
    m_CurrentLevel.pop_back();
    return true;
}

bool CLevelEditor::DecreaseXSize(void)
{
    if (iWorldWidth == 32)
        return false;

    for (int i = 0; i < iWorldHeight; ++i)
    {
        if (m_CurrentLevel[i].back().iTileID > 0)
            return false;
    }

    --iWorldWidth;
    for (int i = 0; i < iWorldHeight; ++i)
    {
        m_CurrentLevel[i].pop_back();
    }
    return true;
}

bool CLevelEditor::IncreaseYSize(void)
{
    ++iWorldHeight;

    sCell cell;
    std::vector<sCell> newRow;
    for (int i = 0; i < iWorldWidth; ++i)
    {
        newRow.push_back(cell);
    }
    m_CurrentLevel.push_back(newRow);
    return true;
}

/**
@brief Returns the Cell of the index specified
*/
sCell CLevelEditor::GetCell(unsigned int x, unsigned int y, bool bInvert)
{
    if (bInvert)
        return m_CurrentLevel[iWorldHeight - y - 1][x];
    else
        return m_CurrentLevel[y][x];
}

/**
@brief Updates the Cell in the level editor given an x and y index
*/
void CLevelEditor::UpdateCell(unsigned int x, unsigned int y, int TileID, bool bInvert)
{
    if (bInvert)
        m_CurrentLevel[iWorldHeight - y - 1][x].iTileID = TileID;
    else
        m_CurrentLevel[y][x].iTileID = TileID;
}


void CLevelEditor::RenderBackground(void) {
    if (cBackgroundEntity) {
        //Parallax background math
        glm::vec2 allowance = glm::vec2((vUVCoords.x - 2), (vUVCoords.y - 2)); //Get allowance for offsetting

        //Scaling of allowance. 0.f to 1.f
        allowance.x *= vAllowanceScale.x;
        allowance.y *= vAllowanceScale.y;

        //Get beginning and end (In terms of how far the camera can go
        glm::vec2 beginning = glm::vec2((float(cSettings->NUM_TILES_XAXIS) / 2.f) - 1.f, (float(cSettings->NUM_TILES_YAXIS) / 2.f) - 1.f);
        glm::vec2 end = glm::vec2(iWorldWidth, iWorldHeight) - beginning;

        //Scaling of position
        glm::vec2 total = end - beginning;
        glm::vec2 curr = camera->getCurrPos() - beginning;

        float uvSpaceX = (curr.x / total.x) * allowance.x;
        float uvSpaceY = (curr.y / total.y) * allowance.y;

        uvSpaceX -= allowance.x / 2.f;
        uvSpaceY -= allowance.y / 2.f;

        glm::vec2 uvSpace = -glm::vec2(uvSpaceX, uvSpaceY);

        cBackgroundEntity->vTransform = uvSpace;
        cBackgroundEntity->scaleX = vUVCoords.x;
        cBackgroundEntity->scaleY = vUVCoords.y;
        //Rendering
        cBackgroundEntity->PreRender();
        cBackgroundEntity->Render();
        cBackgroundEntity->PostRender();
    }
}
