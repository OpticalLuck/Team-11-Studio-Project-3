#pragma once
#include <string>

//System map
namespace SysMap {
	void ExtractIDs(std::string str, int& textureID, int& objectID);
	bool IsInteger(const std::string& s);
}