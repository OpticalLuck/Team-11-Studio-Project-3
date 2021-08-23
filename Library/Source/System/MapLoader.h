#pragma once
#include <string>

//System map
namespace SysMap {
	void ExtractIDs(std::string str, int& textureID, int& objectID, int& backgroundID);
	bool IsInteger(const std::string& s);
}