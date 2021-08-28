#include "Settings.h"

#include "../Library/Source/System/Debug.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "../SoundController/SoundController.h"

using namespace std;

CSettings::CSettings(void)
	: pWindow(NULL)
	, logl_root(NULL)
	, m_ImGuiWindow(NULL)
	, screenSize(SCREENSIZE::SSIZE_800x600)
	, bShowCollider(true)
	, MASTER_VOLUME(100.f)
	, BGM_VOLUME(100.f)
	, SFX_VOLUME(100.f)
	, bBGM_Sound(true)
	, bSFX_Sound(true)
	, NUM_TILES_XAXIS(32)
	, NUM_TILES_YAXIS(24)
	, TILE_WIDTH(0.0625f)
	, TILE_HEIGHT(0.08333f)
	, NUM_STEPS_PER_TILE_XAXIS(16.0f)
	, NUM_STEPS_PER_TILE_YAXIS(16.0f)
	, MICRO_STEP_XAXIS(0.015625f)
	, MICRO_STEP_YAXIS(0.0208325f)
{
	iWindowWidth = 800;
	iWindowHeight = 600;

	switch (screenSize)
	{
	case SCREENSIZE::SSIZE_800x600:
		iWindowWidth = 800;
		iWindowHeight = 600;
		break;
	case SCREENSIZE::SSIZE_1024x768:
		iWindowWidth = 1024;
		iWindowHeight = 768;
		break;
	case SCREENSIZE::SSIZE_1400x1050:
		iWindowWidth = 1400;
		iWindowHeight = 1050;
		break;
	case SCREENSIZE::SSIZE_1600x1200:
		iWindowWidth = 1600;
		iWindowHeight = 1200;
		break;
	}

	LoadSettings();
}


CSettings::~CSettings(void)
{
	SaveSettings();
	m_ImGuiWindow = NULL;
}


/**
@brief Convert an index number of a tile to a coordinate in UV Space
*/
float CSettings::ConvertIndexToUVSpace(const AXIS sAxis, const int iIndex, const bool bInvert, const float fOffset)
{
	float fResult = 0.0f;
	if (sAxis == CSettings::AXIS::x)
	{
		fResult = -1.0f + (float)iIndex * TILE_WIDTH + TILE_WIDTH / 2.0f + fOffset;
	}
	else if (sAxis == CSettings::AXIS::y)
	{
		if (bInvert)
			fResult = 1.0f - (float)(iIndex + 1) * TILE_HEIGHT + TILE_HEIGHT / 2.0f + fOffset;
		else
			fResult = -1.0f + (float)iIndex * TILE_HEIGHT + TILE_HEIGHT / 2.0f + fOffset;
	}
	else if (sAxis == CSettings::AXIS::z)
	{
		// Not used in here
	}
	else
	{
		cout << "Unknown axis" << endl;
	}
	return fResult;
}

glm::vec2 CSettings::ConvertIndexToUVSpace(const glm::vec2 pos)
{
	glm::vec2 output = pos;

	//ORIGINAL ONE
	output.x = ((float)output.x + 0.5f - (0.5f * NUM_TILES_XAXIS)) / (0.5f * NUM_TILES_XAXIS);
	output.y = ((float)output.y + 0.5f - (0.5f * NUM_TILES_YAXIS)) / (0.5f * NUM_TILES_YAXIS);

	//NEW ONE
	//output.x = (((pos.x + 0.5) / NUM_TILES_XAXIS) * 2) - 1;
	//output.y = (((pos.y + 0.5)/ NUM_TILES_YAXIS) * 2) - 1;

	return output;
}
glm::vec2 CSettings::ConvertIndexToScreenSpace(const glm::vec2 pos,const glm::vec2 transform)
{
	glm::vec2 screenspace = pos;
	glm::vec2 worldspace = transform;
	screenspace.x = (float)(worldspace.x / NUM_TILES_XAXIS) * iWindowWidth;
	screenspace.y = (float)(worldspace.y / NUM_TILES_YAXIS) * iWindowHeight;

	return screenspace;
}

float CSettings::ConvertIndexToXScreenSpace(const float posX, const float transformX)
{
	float screenspace = posX;
	float worldspace = transformX;
	screenspace = (float)(worldspace / NUM_TILES_XAXIS) * iWindowWidth;

	return screenspace;
}

float CSettings::ConvertIndexToYScreenSpace(const float posY, const float transformY)
{
	float screenspace = posY;
	float worldspace = transformY;
	screenspace = (float)(worldspace / NUM_TILES_YAXIS) * iWindowHeight;

	return screenspace;
}

glm::vec2 CSettings::ConvertUVSpaceToIndex(const glm::vec2 pos)
{
	glm::vec2 output = pos;
	output.x = (float)output.x * (0.5f * NUM_TILES_XAXIS) + (0.5f * NUM_TILES_XAXIS);
	output.y = (float)output.y * (0.5f * NUM_TILES_YAXIS) + (0.5f * NUM_TILES_YAXIS);

	return output;
}

glm::vec2 CSettings::GetScreenSize()
{
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	glm::vec2 screensize = glm::vec2(mode->width, mode->height);
	return screensize;
}


glm::vec2 CSettings::GetWindowSize()
{
	switch (screenSize)
	{
	case SCREENSIZE::SSIZE_800x600:
		return glm::vec2(800, 600);
	case SCREENSIZE::SSIZE_1024x768:
		return glm::vec2(1024, 768);
	case SCREENSIZE::SSIZE_1400x1050:
		return glm::vec2(1400, 1050);
	case SCREENSIZE::SSIZE_1600x900:
		return glm::vec2(1600, 900);
	case SCREENSIZE::SSIZE_1600x1200:
		return glm::vec2(1600, 1050);
	default:
		DEBUG_MSG("SCREENSIZE TYPE INVALID, REVERTING TO DEFAULT VALUE");
		screenSize = SCREENSIZE::SSIZE_800x600;
		return glm::vec2(800, 600);
	}
}

void CSettings::LoadSettings()
{
	std::ifstream cfile;

	cfile.open("Settings.txt");

	if (!cfile)
	{
		cout << "Unable to open Settings file";
		exit(1);
	}

	std::string cLine;
	while (getline(cfile, cLine))
	{
		std::string title = cLine.substr(0, cLine.find(":"));
		std::string value = cLine.substr(cLine.find(":") + 1, cLine.length());

		if (title == "resolution")
		{
			int horizontal = stoi(value.substr(0, value.find('x')));
			int vertical = stoi(value.substr(value.find('x') + 1, value.length()));

			if (horizontal <= 800)
			{
				screenSize = SCREENSIZE::SSIZE_800x600;
			}
			else if (horizontal <= 1024)
			{
				screenSize = SCREENSIZE::SSIZE_1024x768;
			}
			else if (horizontal <= 1400)
			{
				screenSize = SCREENSIZE::SSIZE_1400x1050;
			}
			else if (horizontal <= 1600)
			{
				if(vertical <= 900)
					screenSize = SCREENSIZE::SSIZE_1600x900;
				else if (vertical <= 1200)
					screenSize = SCREENSIZE::SSIZE_1600x1200;
				else
					screenSize = SCREENSIZE::SSIZE_1600x1200;
			}
			else
			{
				screenSize = SCREENSIZE::SSIZE_800x600;
			}
		}
		else if (title == "master-volume")
		{
			MASTER_VOLUME = (float)stoi(value);
		}
		else if (title == "bgm-volume")
		{
			BGM_VOLUME = (float)stoi(value);
		}
 		else if (title == "sfx-volume")
		{
			SFX_VOLUME = (float)stoi(value);
		}
	}
	cfile.close();

	UpdateWindowSize();
	//Sound will be updated in introstate
}

void CSettings::SaveSettings()
{
	std::string filename = "Settings.txt";
	std::string tempfilename = "Settings_temp.txt";

	std::ifstream cfile;
	cfile.open(filename);
	std::ofstream tempfile(tempfilename);

	std::wstring wtempfilename = to_wide(tempfilename);
	const wchar_t* fileLPCWSTR = wtempfilename.c_str();
	int attr = GetFileAttributes(fileLPCWSTR);
	if ((attr & FILE_ATTRIBUTE_HIDDEN) == 0)
	{
		SetFileAttributes(fileLPCWSTR, attr | FILE_ATTRIBUTE_HIDDEN);
	}

	if (!cfile)
	{
		cout << "Unable to open Settings file";
		exit(1);
	}

	std::string cLine;
	while (getline(cfile, cLine))
	{
		std::string title = cLine.substr(0, cLine.find(":"));

		if (title == "resolution")
		{
			glm::vec2 size = GetWindowSize();
			std::stringstream screensize;
			screensize << size.x << 'x' << size.y;

			cLine.replace(cLine.find(":") + 1, cLine.length(), screensize.str());
		}
		else if (title == "master-volume")
		{
			cLine.replace(cLine.find(":") + 1, cLine.length(), std::to_string((int)MASTER_VOLUME));
		}
		else if (title == "bgm-volume")
		{
			cLine.replace(cLine.find(":") + 1, cLine.length(), std::to_string((int)BGM_VOLUME));
		}
		else if (title == "sfx-volume")
		{
			cLine.replace(cLine.find(":") + 1, cLine.length(), std::to_string((int)SFX_VOLUME));
		}

		tempfile << cLine << endl;
	}

	//must be closed before editing... probably read and write shit
	cfile.close();
	remove(filename.c_str());

	tempfile.close();

	//set attribute before renaming as attr is attached to tempfilename
	if ((attr & FILE_ATTRIBUTE_NORMAL) == 0)
	{
		SetFileAttributes(fileLPCWSTR, attr | FILE_ATTRIBUTE_NORMAL);
	}
	int yes = rename(tempfilename.c_str(), filename.c_str());
}

void CSettings::UpdateSoundSettings()
{
	UpdateMasterVolume();
	UpdateBGMVolume(BGM_VOLUME);
	UpdateSFXVolume(SFX_VOLUME);
}

void CSettings::UpdateMasterVolume()
{
	CSoundController::GetInstance()->SetMasterVolume(MASTER_VOLUME * 0.01f);
}

void CSettings::UpdateBGMVolume(float BGM_VOLUME)
{
	for (int i = SOUND_ID::BGM_START + 1; i < SOUND_ID::BGM_TOTAL; i++)
	{
		CSoundController::GetInstance()->SetVolumeByID(i, BGM_VOLUME * 0.01f);
	}
	CSoundController::GetInstance()->UpdatePlayBackVolume(BGM_VOLUME * 0.01f);
}

void CSettings::UpdateSFXVolume(float SFX_VOLUME)
{
	for (int i = SOUND_ID::SOUND_START + 1; i < SOUND_ID::SOUND_TOTAL; i++)
		CSoundController::GetInstance()->SetVolumeByID(i, SFX_VOLUME * 0.01f);
}


// Update the specifications of the map
void CSettings::UpdateSpecifications(void)
{
	TILE_WIDTH = 2.0f / NUM_TILES_XAXIS;	// 0.0625f;
	TILE_HEIGHT = 2.0f / NUM_TILES_YAXIS;	// 0.08333f;

	MICRO_STEP_XAXIS = TILE_WIDTH / NUM_STEPS_PER_TILE_XAXIS;
	MICRO_STEP_YAXIS = TILE_HEIGHT / NUM_STEPS_PER_TILE_YAXIS;
}

void CSettings::UpdateWindowSize()
{
	switch (screenSize)
	{
	case SCREENSIZE::SSIZE_800x600:
		iWindowWidth = 800;
		iWindowHeight = 600;
		break;
	case SCREENSIZE::SSIZE_1024x768:
		iWindowWidth = 1024;
		iWindowHeight = 768;
		break;
	case SCREENSIZE::SSIZE_1400x1050:
		iWindowWidth = 1400;
		iWindowHeight = 1050;
		break;
	case SCREENSIZE::SSIZE_1600x1200:
		iWindowWidth = 1600;
		iWindowHeight = 1200;
		break;
	case SCREENSIZE::SSIZE_1600x900:
		iWindowWidth = 1600;
		iWindowHeight = 900;
		break;
	}

	if (pWindow)
	{
		glfwSetWindowSize(pWindow, iWindowWidth, iWindowHeight);

		glm::vec2 screensize = CSettings::GetInstance()->GetScreenSize();
		glm::vec2 windowposition = glm::vec2(screensize.x / 2 -iWindowWidth / 2, screensize.y / 2 - iWindowHeight / 2);
		// Set OpenGL window position
		glfwSetWindowPos(pWindow, (int)windowposition.x, (int)windowposition.y);

	}
}
