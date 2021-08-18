#include "LevelGrid.h"

#include "GameControl/Settings.h"

#include "../Scene2D/Camera2D.h"

CLevelGrid::CLevelGrid()
	: iWorldWidth(32)
	, iWorldHeight(24)
    , vec2Dimensions(1.f)
    , vec4Colour(1.f)
    , fLineWidth(1.2f)
    , transform(1.f)
{
	sLineShaderName = "LineShader";
}

CLevelGrid::~CLevelGrid()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void CLevelGrid::Init(unsigned int iWorldWidth, unsigned int iWorldHeight)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

    glm::vec2 vec2Dimensions = glm::vec2(1, 1) * 0.5f;
    glm::vec4 vec4Colour = glm::vec4(1, 1, 1, 1);

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

    this->iWorldWidth = iWorldWidth;
    this->iWorldHeight = iWorldHeight;

}

void CLevelGrid::PreRender(void)
{
    // Use the shader defined for this class
    CShaderManager::GetInstance()->Use(sLineShaderName);
}

void CLevelGrid::Render(void)
{
    glm::vec2 offset = glm::vec2(float(CSettings::GetInstance()->NUM_TILES_XAXIS / 2.f) - 0.5f, float(CSettings::GetInstance()->NUM_TILES_YAXIS / 2.f) - 0.5f);
    glm::vec2 cameraPos = Camera2D::GetInstance()->getCurrPos();

    for (unsigned int uiRow = 0; uiRow < iWorldHeight; ++uiRow)
    {
        for (unsigned int uiCol = 0; uiCol < iWorldWidth; ++uiCol)
        {

            glm::vec2 objCamPos = glm::vec2(uiCol, uiRow) - cameraPos + offset;
            glm::vec2 actualPos = CSettings::GetInstance()->ConvertIndexToUVSpace(objCamPos);

            float clampX = 1.01f;
            float clampY = 1.01f;
            if (actualPos.x <= -clampX || actualPos.x >= clampX || actualPos.y <= -clampY || actualPos.y >= clampY)
                continue;

            transform = glm::mat4(1.f);
            transform = glm::translate(transform, glm::vec3(actualPos.x, actualPos.y, 0.f));
            transform = glm::scale(transform, glm::vec3(CSettings::GetInstance()->TILE_WIDTH, CSettings::GetInstance()->TILE_HEIGHT, 1.f));

            CShaderManager::GetInstance()->activeShader->setMat4("transform", transform);

            glBindVertexArray(VAO);
            glDrawArrays(GL_LINE_LOOP, 0, 6);
            glBindVertexArray(0);

        }
    }
}

void CLevelGrid::PostRender(void)
{
}

void CLevelGrid::SetLineShader(const std::string& name)
{
    sLineShaderName = name;
}
