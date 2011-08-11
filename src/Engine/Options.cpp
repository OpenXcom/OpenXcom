/*
 * Copyright 2010 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Options.h"
#include <map>
#include <sstream>
#include <fstream>
#include "yaml.h"
#include "Exception.h"

namespace OpenXcom
{
namespace Options
{
	
std::string _version = "0.2";
std::string _dataFolder = "./DATA/";
std::string _userFolder = "./USER/";
std::map<std::string, std::string> _options;

/**
 * Creates a default set of options based on the system.
 */
void create()
{
#ifdef DINGOO
	setInt("displayWidth", 320);
	setInt("displayHeight", 200);
	setBool("fullscreen", true);
#else
	setInt("displayWidth", 640);
	setInt("displayHeight", 400);
	setBool("fullscreen", false);
#endif
}

/**
 * Loads options from a YAML file.
 * @param filename YAML filename.
 */
void load(const std::string &filename)
{
	std::string s = Options::getUserFolder() + filename + ".cfg";
	std::ifstream fin(s.c_str());
	if (!fin)
	{
		throw Exception("Failed to load options");
	}
    YAML::Parser parser(fin);
	YAML::Node doc;

    parser.GetNextDocument(doc);
	std::string v;
	doc["version"] >> v;
	if (v != _version)
	{
		throw Exception("Version mismatch");
	}
	doc["options"] >> _options;

	fin.close();
}

/**
 * Saves options to a YAML file.
 * @param filename YAML filename.
 */
void save(const std::string &filename)
{
	std::string s = Options::getUserFolder() + filename + ".cfg";
	std::ofstream sav(s.c_str());
	if (!sav)
	{
		throw Exception("Failed to save options");
	}
	YAML::Emitter out;

	out << YAML::BeginDoc;
	out << YAML::BeginMap;
	out << YAML::Key << "version" << YAML::Value << _version;
	out << YAML::Key << "options" << YAML::Value << _options;
	out << YAML::EndMap;

	sav << out.c_str();
	sav.close();
}

std::string getVersion()
{
	return _version;
}

std::string getDataFolder()
{
	return _dataFolder;
}

void setDataFolder(std::string folder)
{
	_dataFolder = folder;
}

std::string getUserFolder()
{
	return _userFolder;
}

void setUserFolder(std::string folder)
{
	_userFolder = folder;
}

std::string getString(std::string id)
{
	return _options[id];
}

int getInt(std::string id)
{
	std::stringstream ss;
	int value;
	ss << std::dec << _options[id];
	ss >> std::dec >> value;
	return value;
}

bool getBool(std::string id)
{
	std::stringstream ss;
	bool value;
	ss << std::boolalpha << _options[id];
	ss >> std::boolalpha >> value;
	return value;
}

void setString(std::string id, std::string value)
{
	_options[id] = value;
}

void setInt(std::string id, int value)
{
	std::stringstream ss;
	ss << std::dec << value;
	_options[id] = ss.str();
}

void setBool(std::string id, bool value)
{
	std::stringstream ss;
	ss << std::boolalpha << value;
	_options[id] = ss.str();
}

}
}
