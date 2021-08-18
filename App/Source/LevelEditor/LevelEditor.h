#pragma once

// Include Singleton Pattern
#include "DesignPatterns/SingletonTemplate.h"

#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

#include <GLFW/glfw3.h>

// Include the RapidCSV
#include "System/rapidcsv.h"

#include "Primitives/MeshBuilder.h"

#include "GameControl/Settings.h"

#include "RenderControl/ShaderManager.h"

#include "../TextureManager/TextureManager.h"

#include <string>

#include <vector>
#include "../Scene2D/Camera2D.h"

struct sCell
{
    int iTileID = 0;
};

struct Level
{
    std::string LevelName = "";
    std::string LevelPath = "";
};

class CLevelEditor : public CSingletonTemplate<CLevelEditor>
{
    friend CSingletonTemplate<CLevelEditor>;
public:

    unsigned int iWorldWidth;
    unsigned int iWorldHeight;

    void Init();

    void PreRender();
    void Render();
    void PostRender();

    void CreateLevel(std::string levelName, unsigned int iWorldWidth = 32, unsigned int iWorldHeight = 24);
    void LoadLevel(const char* filePath);
    void LoadExistingLevels(void);
    sCell GetCell(unsigned int x, unsigned int y, bool bInvert = true);
    void UpdateCell(unsigned int x, unsigned int y, int TileID, bool bInvert = true);
    void SetShader(const std::string& _name);

    std::vector<Level> GetLevels(void);

protected:

    // Application Settings
    CSettings* cSettings;

    // OpenGL Locations
    unsigned int VAO, VBO, EBO;
    unsigned int quadVAO, quadVBO, quadEBO;

    // The texture ID in OpenGL
    unsigned int iTextureID;

    // Render Control
    CTextureManager*    cTextureManager;                // Texture Manager Instance
    std::string         sShaderName;                    // Name of Shader Program instance
    glm::mat4           transform;                      // A transformation matrix for controlling where to render the entities
    CMesh*              quadMesh;                       // Mesh to draw
    Camera2D*           camera;                         // Camera handler

    // Document Variables
    rapidcsv::Document  doc;                            // CSV document of the level
    std::string         levelFolderPath;                // Path to the folder that contains the level CSVs

    // Level Editor Variables
    std::vector<std::vector<sCell>> m_CurrentLevel;     // Dynamic Array that stores the map of the currently editted level
    std::vector<Level>              m_Levels;           // Vector of stored levels
    Level                           currentLevel;       // Current Level data

    CLevelEditor();
    ~CLevelEditor();

    void GenerateQuadVAO(void);

};