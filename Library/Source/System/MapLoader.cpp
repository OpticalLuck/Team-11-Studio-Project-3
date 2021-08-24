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

void SysMap::ExtractIDs(std::string str, int& textureID, int& objectID, int& backgroundID) {
	//If there are no ; present (Basically only one value in cell like "100")
	if (IsInteger(str)) {
		textureID = stoi(str);
		objectID = 0;
		
		return;
	}

	//Conversion for if there are more than 1 value present (100;2)
	std::stringstream ss(str);

	std::vector<std::string> slashChk;
	std::vector<int> values;

	while (ss.good()) {
		std::string substr;
		getline(ss, substr, '/');

		slashChk.push_back(substr);
	}

	if (slashChk.size() > 1 && IsInteger(slashChk[1]))
		backgroundID = std::stoi(slashChk[1]);

	//Reinitialise ss
	ss.str("");
	ss.clear();
	ss.str(slashChk[0]);

	while (ss.good()) {
		std::string substr;
		getline(ss, substr, ';');

		//Convert value from string to int if possible.
		values.push_back(std::stoi(substr));
	}

	textureID = values[0];

	if (values.size() > 1)
		objectID = values[1];
}