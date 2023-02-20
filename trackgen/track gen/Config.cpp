#include "Config.h"

#include <fstream>
#include <string>
#include <queue>
#include <algorithm>

int Config::initConfig()
{
	std::fstream config_file;

	std::queue < std::string > lines;

	config_file.open("config.txt", std::ios::in);
	if (!config_file.good()) return 0;

	while (!config_file.eof())
	{
		std::string line;
		
		std::getline(config_file, line);
		lines.push(line);
	}

	config_file.close();

	while(!lines.empty())
	{
		std::string key = "";
		std::string value = "";
		std::string line = "";

		size_t equal_index = 0;
		size_t hash_index = 0;

		line = lines.front();
		lines.pop();

		hash_index = line.find('#');
		if (hash_index != std::string::npos) line.erase(line.begin() + hash_index, line.end());

		line.erase(std::remove_if(line.begin(), line.end(), [](char x) { return std::isspace(x); }), line.end());
		if (line.size() == 0) continue;

		equal_index = line.find('=');
		if (equal_index == std::string::npos) continue;
		if (equal_index == 0 || equal_index == line.size() - 1) continue;

		key = line.substr(0, equal_index);
		value = line.substr(equal_index + 1);

		for (char& c : value) if (c == '/') c = ' '; 

		values.emplace(key, value);
	}

	initalized = true;

	return 1;
}

std::string Config::get(const std::string& const setting)
{
	if (!initalized) initConfig();

	return values.find(setting)->second;
}