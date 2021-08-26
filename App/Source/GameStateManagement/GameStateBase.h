#pragma once

/**
 CGameStateBase
 @brief This is an Abstract Class. Derive from this class to implement it.
 By: Toh Da Jun
 Date: July 2021
 */

#include <string>
class CGameStateBase
{
public:
	// Constructor
	CGameStateBase(void) {}
	// Destructor
	virtual ~CGameStateBase(void) {}

	// These are pure virtual functions
	virtual bool Init() = 0;
	virtual bool Update(const double dElapsedTime) = 0;
	virtual void Render() = 0;
	virtual void Destroy() = 0;

	virtual bool ImGuiRender() = 0;
};

struct ButtonData
{
	std::string fileName;
	unsigned textureID = 0;
};