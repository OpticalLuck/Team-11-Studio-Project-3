#include "MapLoader.h"

#include <sstream>
#include <vector>
#include <iostream>

bool SysMap::IsInteger(const std::string& s) {
	if (s.empty() || (!isdigit(s[0]) && s[0] != '-'))
		return false; //Return false if its empty of if string is not a digit

	char* p;
	long int temp = strtol(s.c_str(), &p, 10); //Parsing through string and check for index that is not an integer

	return (*p == 0);
}

void SysMap::ExtractIDs(std::string str, int& textureID, int& objectID) {
	//If there are no ; present (Basically only one value in cell like "100")
	if (IsInteger(str)) {
		textureID = stoi(str);
		objectID = 0;
		return;
	}

	//Conversion for if there are more than 1 value present (100;2)
	std::stringstream ss(str);
	std::vector<int> values;

	while (ss.good()) {
		std::string substr;
		getline(ss, substr, ';');

		//Convert valaue from string to int if possible.
		values.push_back(std::stoi(substr));
	}

	textureID = values[0];
	objectID = values[1];
}