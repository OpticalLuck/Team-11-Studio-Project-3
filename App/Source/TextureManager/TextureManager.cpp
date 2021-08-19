#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

#include <GLFW/glfw3.h>

#include "TextureManager.h"
#include "System/ImageLoader.h"

#include <iostream>

CTextureManager::CTextureManager()
{
}

CTextureManager::~CTextureManager()
{
}


bool CTextureManager::Init(void)
{

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

    if (LoadTexture("Image/Boulder.png", 150) == false)
    {
        std::cout << "Failed to load Boulder tile texture" << std::endl;
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