#include "Settings.h"

#include <iostream>
using namespace std;

CSettings::CSettings(void)
	: pWindow(NULL)
	, logl_root(NULL)
	, m_ImGuiWindow(NULL)
	, screenSize(SSIZE_800x600)
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
	switch (screenSize)
	{
	case SSIZE_800x600:
		iWindowWidth = 800;
		iWindowHeight = 600;
		break;
	case SSIZE_1024x768:
		iWindowWidth = 1024;
		iWindowHeight = 768;
		break;
	case SSIZE_1400x1050:
		iWindowWidth = 1400;
		iWindowHeight = 1050;
		break;
	case SSIZE_1600x1200:
		iWindowWidth = 1600;
		iWindowHeight = 1200;
		break;
	}
}


CSettings::~CSettings(void)
{
	m_ImGuiWindow = NULL;
}


/**
@brief Convert an index number of a tile to a coordinate in UV Space
*/
float CSettings::ConvertIndexToUVSpace(const AXIS sAxis, const int iIndex, const bool bInvert, const float fOffset)
{
	float fResult = 0.0f;
	if (sAxis == x)
	{
		fResult = -1.0f + (float)iIndex * TILE_WIDTH + TILE_WIDTH / 2.0f + fOffset;
	}
	else if (sAxis == y)
	{
		if (bInvert)
			fResult = 1.0f - (float)(iIndex + 1) * TILE_HEIGHT + TILE_HEIGHT / 2.0f + fOffset;
		else
			fResult = -1.0f + (float)iIndex * TILE_HEIGHT + TILE_HEIGHT / 2.0f + fOffset;
	}
	else if (sAxis == z)
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
	output.x = (output.x - (0.5 * NUM_TILES_XAXIS)) / (0.5 * NUM_TILES_XAXIS);
	output.y = (output.y - (0.5 * NUM_TILES_YAXIS)) / (0.5 * NUM_TILES_YAXIS);

	//NEW ONE
	//output.x = (((pos.x + 0.5) / NUM_TILES_XAXIS) * 2) - 1;
	//output.y = (((pos.y + 0.5)/ NUM_TILES_YAXIS) * 2) - 1;

	return output;
}

glm::vec2 CSettings::ConvertUVSpaceToIndex(const glm::vec2 pos)
{
	glm::vec2 output = pos;
	output.x = output.x * (0.5 * NUM_TILES_XAXIS) + (0.5 * NUM_TILES_XAXIS);
	output.y = output.y * (0.5 * NUM_TILES_YAXIS) + (0.5 * NUM_TILES_YAXIS);

	return output;
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
	case SSIZE_800x600:
		iWindowWidth = 800;
		iWindowHeight = 600;
		break;
	case SSIZE_1024x768:
		iWindowWidth = 1024;
		iWindowHeight = 768;
		break;
	case SSIZE_1400x1050:
		iWindowWidth = 1400;
		iWindowHeight = 1050;
		break;
	case SSIZE_1600x1200:
		iWindowWidth = 1600;
		iWindowHeight = 1200;
		break;
	case SSIZE_1600x900:
		iWindowWidth = 1600;
		iWindowHeight = 900;
		break;
	}
	glfwSetWindowSize(pWindow, iWindowWidth, iWindowHeight);
}
