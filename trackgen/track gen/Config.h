#pragma once

#include <SFML/System.hpp>

#include <map>

#include <fstream>
#include <string>
#include <queue>
#include <algorithm>

class Config
{
	public:
		static int initConfig();

		static std::string get(const std::string& const setting);

	private:
		inline static bool initalized = false;

		inline static std::map < std::string, std::string > values;
};