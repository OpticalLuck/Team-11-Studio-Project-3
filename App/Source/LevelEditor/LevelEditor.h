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
#include "../Scene2D/BackgroundEntity.h"

struct sCell
{
    int iTileID = 0;
    int iInteractableID = 0;
    int iBackgroundTileID = 0;
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

    glm::vec2 vAllowanceScale;
    glm::vec2 vUVCoords;
    std::string backgroundPath;
    CBackgroundEntity* cBackgroundEntity;

    void Init();
    
    void PreRender();
    void Render();
    void PostRender();
    void RenderBackground(void);

    // Loading and Creating Functions
    void CreateLevel(std::string levelName, unsigned int iWorldWidth = 32, unsigned int iWorldHeight = 24);  
    bool LoadLevel(const char* filePath);                                                                    
    bool LoadLevelByName(std::string levelName);                                                             
    void LoadExistingLevels(void);                                                                           
    bool LevelExists(std::string levelName);                                                                 
    std::vector<std::vector<sCell>> GetCurrentLevel(void);
    void SetCurrentLevel(std::vector<std::vector<sCell>> levelArr);
    std::vector<Level> GetLevels(void);      
    Level GetCurrentLevelData(void);
    bool SaveMap(void);

    // Resize Map
    bool DecreaseXSize(void);
    bool IncreaseYSize(void);
    bool IncreaseXSize(void);
    bool DecreaseYSize(void);

    // Update Map
    sCell GetCell(unsigned int x, unsigned int y, bool bInvert = false);                
    void UpdateCell(unsigned int x, unsigned int y, int TileID, int InteractableID = 0, bool bInvert = false);  

    // Render Control
    void SetShader(const std::string& _name);

protected:

    // Application Settings
    CSettings* cSettings;

    // OpenGL Locations
    unsigned int VAO, VBO, EBO;

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

    void ResetParams();
};