#include "TextureManager.h"

#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

#include <GLFW/glfw3.h>

#include "System/ImageLoader.h"
#include "../Scene2D/Object2D.h"
#include "System/Debug.h"
#include <iostream>
#include <sstream>

CTextureManager::CTextureManager()
{
}

CTextureManager::~CTextureManager()
{
}


bool CTextureManager::Init(void)
{
        
    if (LoadTexture("Image/Cyborg/Cyborg_Preview.png", PLAYER_TILE) == false)
    {
        DEBUG_MSG("Failed to load player tile texture");
        return false;
    }

    if (LoadTexture("Image/Ground/GroundTile.png", TILE_GROUND) == false)
    {
        DEBUG_MSG("Failed to load ground tile texture");
        return false;
    }
    if (LoadTexture("Image/Ground/GrassTile.png", TILE_GRASS) == false)
    {
        DEBUG_MSG("Failed to load GrassTile tile texture");
        return false;
    }
    if (LoadTexture("Image/Ground/LeftGrassTile.png", TILE_LEFT_GRASS) == false)
    {
        DEBUG_MSG("Failed to load LeftGrassTile tile texture");
        return false;
    }
    if (LoadTexture("Image/Ground/RightGrassTile.png", TILE_RIGHT_GRASS) == false)
    {
        DEBUG_MSG("Failed to load RightGrassTile tile texture");
        return false;
    }
    if (LoadTexture("Image/Ground/TopRightGroundTile.png", TILE_TOP_RIGHT_GROUND) == false) {
        DEBUG_MSG("Failed to load TopRightGrassTile tile texture");
    }

    if (LoadTexture("Image/Walls/wall_1.png", TILE_WALL_1) == false)
    {
        DEBUG_MSG("Failed to load wall_1 tile texture");
        return false;
    }
    if (LoadTexture("Image/Walls/wall_2.png", TILE_WALL_2) == false)
    {
        DEBUG_MSG("Failed to load wall_2 tile texture");
        return false;
    }
    if (LoadTexture("Image/Walls/Wall_3.png", TILE_DARK) == false) {
        DEBUG_MSG("Failed to load Wall_3 tile texture");
        return false;
    }
    if (LoadTexture("Image/Boulder.png", OBSTACLE_BOULDER) == false)
    {
        DEBUG_MSG("Failed to load Boulder tile texture");
        return false;
    }

    if (LoadTexture("Image/items/shuriken.png", BULLETS_SHURIKEN) == false)
    {
        DEBUG_MSG("Failed to load shuriken tile texture");
        return false;
    }
    if (LoadTexture("Image/items/potion.png", CONSUMABLES_POTION) == false)
    {
        DEBUG_MSG("Failed to load potion tile texture");
        return false;
    }
    if (LoadTexture("Image/items/grappling_hook.png", EQUIPMENTS_HOOK) == false)
    {
        DEBUG_MSG("Failed to load grappling hook tile texture");
        return false;
    }
    if (LoadTexture("Image/items/kunai.png", BULLETS_KUNAI) == false)
    {
        DEBUG_MSG("Failed to load kunai tile texture");
        return false;
    }

    if (LoadTexture("Image/items/Scene2D_Lives.tga", UI_LIVES) == false)
    {
        DEBUG_MSG("Failed to load Scene2D_Lives tile texture");
        return false;
    }
    if (LoadTexture("Image/Switches/switch_1_off.png", INTERACTABLE_SWITCH1_OFF) == false)
    {
        DEBUG_MSG("Failed to load switch_1_off tile texture");
        return false;
    }
    if (LoadTexture("Image/Switches/switch_1_on.png", INTERACTABLE_SWITCH1_ON) == false)
    {
        DEBUG_MSG("Failed to load switch_1_on tile texture");
        return false;
    }

    if (LoadTexture("Image/Switches/pressureplate_1_off.png", INTERACTABLE_PRESSURE_PLATE_OFF) == false)
    {
        DEBUG_MSG("Failed to load switch_2_off tile texture");
        return false;
    }
    if (LoadTexture("Image/Switches/pressureplate_1_on.png", INTERACTABLE_PRESSURE_PLATE_ON) == false)
    {
        DEBUG_MSG("Failed to load switch_2_on tile texture");
        return false;
    }

    if (LoadTexture("Image/Portals/portal.png", INTERACTABLE_PORTAL) == false)
    {
        DEBUG_MSG("Failed to load portal15 tile texture");
        return false;
    }


    if (LoadTexture("Image/Portals/portalgate.png", INTERACTABLE_PORTAL_GATE) == false)
    {
        DEBUG_MSG("Failed to load portalgate tile texture");
        return false;
    }
    if (LoadTexture("Image/Portals/portalgate_off.png", INTERACTABLE_PORTAL_GATE_OFF) == false)
    {
        DEBUG_MSG("Failed to load portalgate_off tile texture");
        return false;
    }

    if (LoadTexture("Image/Scene2D_Spikes.tga", INTERACTABLE_SPIKES) == false)
    {
        DEBUG_MSG("Failed to load Scene2D_Spikes tile texture");
        return false;
    }

    if (LoadTexture("Image/altar.png", INTERACTABLE_ALTAR) == false)
    {
        DEBUG_MSG("Failed to load Scene2D_Spa tile texture");
        return false;
    }

    if (LoadTexture("Image/Switches/door_1_closed.png", INTERACTABLE_DOOR_CLOSED) == false)
    {
        DEBUG_MSG("Failed to load Door Closed tile texture");
        return false;
    }

    if (LoadTexture("Image/Switches/door_1_open.png", INTERACTABLE_DOOR_OPEN) == false)
    {
        DEBUG_MSG("Failed to load Door Open tile texture");
        return false;
    }


    if (LoadTexture("Image/Chests/Common_Chest_Closed.png", INTERACTABLE_COMMON_CHEST_CLOSED) == false)
    {
        DEBUG_MSG("Failed to load Common Chest Closed tile texture");
        return false;
    }

    if (LoadTexture("Image/Chests/Common_Chest_Opened.png", INTERACTABLE_COMMON_CHEST_OPENED) == false)
    {
        DEBUG_MSG("Failed to load Rare Chest Open tile texture");
        return false;
    }

    if (LoadTexture("Image/Chests/Rare_Chest_Closed.png", INTERACTABLE_RARE_CHEST_CLOSED) == false)
    {
        DEBUG_MSG("Failed to load Rare Chest Closed tile texture");
        return false;
    }

    if (LoadTexture("Image/Chests/Rare_Chest_Opened.png", INTERACTABLE_RARE_CHEST_OPENED) == false)
    {
        DEBUG_MSG("Failed to load Common Chest Open tile texture");
        return false;
    }

    if (LoadTexture("Image/Checkpoint/checkpoint_inactive.png", INTERACTABLE_CHECKPOINT_INACTIVE) == false)
    {
        DEBUG_MSG("Failed to load Checkpoint Inactive tile texture");
        return false;
    }

    if (LoadTexture("Image/Checkpoint/checkpoint_active.png", INTERACTABLE_CHECKPOINT_ACTIVE) == false)
    {
        DEBUG_MSG("Failed to load Checkpoint Active tile texture");
        return false;
    }

    if (LoadTexture("Image/Lives.png", INTERACTABLE_LIVES) == false) 
    {
        DEBUG_MSG("Failed to lives texture");
        return false;
    }

    if (LoadTexture("Image/Lives_Empty.png", INTERACTABLE_LIVES_EMPTY) == false)
    {
        DEBUG_MSG("Failed to lives empty texture");
        return false;
    }

    if (LoadTexture("Image/Enemy/MushroomPreview.png", ENEMY_1) == false)
    {
        DEBUG_MSG("Failed to load Mushroom tile texture");
        return false;
    }

    if (LoadTexture("Image/Enemy/SnailPreview.png", ENEMY_2) == false)
    {
        DEBUG_MSG("Failed to load Pig tile texture");
        return false;
    }

    if (LoadTexture("Image/Enemy/PauseMachineGun.png", ENEMY_3) == false)
    {
        DEBUG_MSG("Failed to load Snail tile texture");
        return false;
    }

    if (LoadTexture("Image/Enemy/MachineGun.png", ENEMY_4) == false)
    {
        DEBUG_MSG("Failed to load Machine Gun tile texture");
        return false;
    }

    if (LoadTexture("Image/Enemy/PigPreview.png", ENEMY_5) == false)
    {
        DEBUG_MSG("Failed to load Pig tile texture");
        return false;
    }

    if (LoadTexture("Image/Scene2D_EnemyTile.tga", BOSS) == false) 
    {
        DEBUG_MSG("Failed to load boss tile texture");
        return false;
    }

    if (LoadTexture("Image/Objects/Bullet.tga", BULLETS_ENEMY) == false) 
    {
        DEBUG_MSG("Failed to load enemy bullet texture");
        return false;
    }

    if (LoadTexture("Image/items/bomb.png", PROJECTILES_BOMB) == false) 
    {
        DEBUG_MSG("Failed to load bomb texture");
        return false;
    }

    for (int i = 1; i <= 8; i++)
    {
        stringstream lazy;
        lazy.str("");
        lazy << "Image/Futuristic/Tile (" << i << ").png";
        if (LoadTexture(lazy.str().c_str(), TILE_FUTURE_1 + i - 1) == false)
        {
            DEBUG_MSG("Failed to load " << lazy.str());
            return false;
        }
    }
    for (int i = 1; i <= 3; i++)
    {
        stringstream lazy;
        lazy.str("");
        lazy << "Image/Futuristic/Fence (" << i << ").png";
        if (LoadTexture(lazy.str().c_str(), TILE_FUTURE_FENCE_1 + i - 1) == false)
        {
            DEBUG_MSG("Failed to load " << lazy.str());
            return false;
        }
    }
    for (int i = 1; i <= 18; i++)
    {
        stringstream thanksJeryl;
        thanksJeryl.str("");
        thanksJeryl << "Image/Villege/Villege (" << i << ").png";
        if (LoadTexture(thanksJeryl.str().c_str(), TILE_VILLEGE_1 + i - 1) == false)
        {
            DEBUG_MSG("Failed to load " << thanksJeryl.str());
            return false;
        }
    }

    if (LoadTexture("Image/Futuristic/Barrel (1).png", OBSTACLE_FUTURE_BARREL1) == false)
    {
        DEBUG_MSG("Failed to load Barrel (1) texture");
        return false;
    }

    if (LoadTexture("Image/Futuristic/Barrel (2).png", OBSTACLE_FUTURE_BARREL2) == false)
    {
        DEBUG_MSG("Failed to load Barrel (2) texture");
        return false;
    }

    if (LoadTexture("Image/Futuristic/Box.png", OBSTACLE_FUTURE_BOX) == false)
    {
        DEBUG_MSG("Failed to load Box texture");
        return false;
    }

	return true;
}


/**
 @brief Load a texture, assign it a code and store it in MapOfTextureIDs.
 @param filename A const char* variable which contains the file name of the texture
 @param iTextureCode A const int variable which is the texture code.
 */
bool CTextureManager::LoadTexture(const char* filename, const int iTextureCode)
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
	unsigned char* data = cImageLoader->Load(filename, width, height, nrChannels, true);
	if (data)
	{
		if (nrChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if (nrChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		// Generate mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);

		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(std::pair<int, int>(iTextureCode, textureID));
	}
	else
	{
		return false;
	}
	// Free up the memory of the file data read in
	free(data);

	return true;
}