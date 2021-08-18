#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include "LevelEditor.h"

#include <iostream>
#include <experimental/filesystem>

// Include Filesystem
#include "System\filesystem.h"

#include "System/Debug.h"

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
    glm::vec2 offset = glm::vec2(float(cSettings->NUM_TILES_XAXIS / 2.f) - 0.5f, float(cSettings->NUM_TILES_YAXIS / 2.f) - 0.5f);
    glm::vec2 cameraPos = camera->getCurrPos();

    for (unsigned int uiRow = 0; uiRow < iWorldHeight; ++uiRow) 
    {
        for (unsigned int uiCol = 0; uiCol < iWorldWidth; ++uiCol)
        {

            if (uiRow == 20)
            {
                int a;
                a = 1;
            }

            glm::vec2 objCamPos = glm::vec2(uiCol, uiRow) - cameraPos + offset;
            glm::vec2 actualPos = cSettings->ConvertIndexToUVSpace(objCamPos);

            float clampX = 1.01f;
            float clampY = 1.01f;
            // if (actualPos.x <= -clampX || actualPos.x >= clampX || actualPos.y <= -clampY || actualPos.y >= clampY)
                // continue;

            transform = glm::mat4(1.f);
            transform = glm::translate(transform, glm::vec3(actualPos.x, actualPos.y, 0.f));
            /*transform = glm::translate(transform, glm::vec3(
                cSettings->ConvertIndexToUVSpace(cSettings->x, currObj.indexSpace.x, false, 0),
                cSettings->ConvertIndexToUVSpace(cSettings->y, currObj.indexSpace.y, false),
                0.f
            ));*/

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

            //transform = glm::scale(transform, glm::vec3(CSettings::GetInstance()->TILE_WIDTH, CSettings::GetInstance()->TILE_HEIGHT, 1.f));

            //CShaderManager::GetInstance()->Use("LineShader");
            //CShaderManager::GetInstance()->activeShader->setMat4("transform", transform);

            //glBindVertexArray(quadVAO);
            //glDrawArrays(GL_LINE_LOOP, 0, 6);
            //glBindVertexArray(0);

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
    for (const auto& file : std::experimental::filesystem::directory_iterator(levelFolderPath))
    {
        Level level;
        level.LevelName = file.path().filename().generic_string();
        level.LevelPath = file.path().generic_string();
        m_Levels.push_back(level);

        DEBUG_MSG("Loaded " << file.path().filename());
    }
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