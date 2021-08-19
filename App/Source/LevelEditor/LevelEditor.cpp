#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include "LevelEditor.h"

#include <iostream>
#include <experimental/filesystem>

// Include Filesystem
#include "System\filesystem.h"

#include "System/Debug.h"

namespace fs = std::experimental::filesystem;

CLevelEditor::CLevelEditor()
    : iWorldWidth(32)
    , iWorldHeight(24)
    , quadMesh(NULL)
    , cSettings(NULL)
    , camera(NULL)
    , cTextureManager(NULL)
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

/**
@brief Initialise this instance
*/
void CLevelEditor::Init()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GenerateQuadVAO();

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

            if (m_CurrentLevel[uiRow][uiCol].iTileID > 1)
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

    DEBUG_MSG("Created New Level: " << levelName);
}

/**
@brief Loads an existing CSV into the level editor
*/
void CLevelEditor::LoadLevel(const char* filePath)
{
    // Load the Level CSV
    doc = rapidcsv::Document(FileSystem::getPath(filePath).c_str());

    // Set World Width and Height
    iWorldWidth = doc.GetColumnCount();
    iWorldHeight = doc.GetRowCount();

    // Iterate through the Level Editor Map and set the values of the corresponding indexes
    for (int iRow = 0; iRow < iWorldHeight; ++iRow)
    {
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

/**
@brief Loads existing CSV files into the m_Levels vector
*/
void CLevelEditor::LoadExistingLevels(void)
{
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

bool CLevelEditor::LevelExists(std::string levelName)
{
    for (const auto& file : fs::directory_iterator(levelFolderPath))
        if (levelName + ".csv" == file.path().filename().generic_string())
            return true;

    return false;
}

bool CLevelEditor::IncreaseXSize()
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

bool CLevelEditor::DecreaseXSize()
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
@brief Generates the VAO for the grid quad
*/
void CLevelEditor::GenerateQuadVAO(void)
{
    glm::vec2 vec2Dimensions = glm::vec2(1, 1) * 0.5f;
    glm::vec4 vec4Colour = glm::vec4(0, 0, 0, 0.3);

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

    glLineWidth(1.f);
}