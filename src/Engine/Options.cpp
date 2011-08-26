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
#include "SDL_mixer.h"
#include <map>
#include <sstream>
#include <fstream>
#include "yaml.h"
#include "Exception.h"
#include "CrossPlatform.h"

namespace OpenXcom
{
namespace Options
{
	
std::string _version = "0.3";
std::string _dataFolder = "";
std::string _userFolder = "";
std::map<std::string, std::string> _options;

/**
 * Creates a default set of options based on the system.
 */
void createDefault()
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
	setInt("soundVolume", MIX_MAX_VOLUME);
	setInt("musicVolume", MIX_MAX_VOLUME);
	setInt("battleScrollSpeed", 24); // 40, 32, 24, 16, 8
	setInt("battleScrollType", SCROLL_AUTO);
	setInt("battleFireSpeed", 20); // 30, 25, 20, 15, 10, 5
	setInt("battleXcomSpeed", 40); // 60, 50, 40, 30, 20, 10
	setInt("battleAlienSpeed", 40); // 60, 50, 40, 30, 20, 10
	// set to true if you want to play with the alternative grenade handling
	setBool("battleAltGrenade", false);
}

/**
 * Loads options from a set of command line arguments,
 * in the format "-option value".
 * @param argc Number of arguments.
 * @param args Array of argument strings.
 */
void loadArgs(int argc, char** args)
{
	for (int i = 1; i < argc; ++i)
	{
		if (argc > i + 1)
		{
			std::string arg = args[i];
			if (arg[0] == '-' && argc > i + 1)
			{
				std::map<std::string, std::string>::iterator it = _options.find(arg.substr(1, arg.length()-1));
				if (it != _options.end())
				{
					it->second = args[i+1];
				}
			}
			else if (arg == "-data")
			{
				_dataFolder = args[i+1];
			}
			else if (arg == "-user")
			{
				_userFolder = args[i+1];
			}
		}
	}
	if (_userFolder != "")
	{
		load();
	}
}

/**
 * Handles the initialization of setting up default options
 * and finding and loading any existing ones.
 * @param argc Number of arguments.
 * @param args Array of argument strings.
 */
void init(int argc, char** args)
{
	createDefault();
	loadArgs(argc, args);
	if (_dataFolder == "")
	{
		_dataFolder = CrossPlatform::findDataFolder(true);
		// Missing data folder is handled in StartState
	}
	if (_userFolder == "")
	{
		_userFolder = CrossPlatform::findUserFolder(true);
		// Create user folder and save options
		if (_userFolder == "")
		{
			_userFolder = CrossPlatform::findUserFolder(false);
			CrossPlatform::createFolder(Options::getUserFolder().c_str());
			save();
		}
		// Load existing options
		else
		{
			load();
		}
	}
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
		//throw Exception("Failed to load options");
		return;
	}
    YAML::Parser parser(fin);
	YAML::Node doc;

    parser.GetNextDocument(doc);
	for (YAML::Iterator i = doc.begin(); i != doc.end(); ++i)
	{
		std::string key, value;
		i.first() >> key;
		i.second() >> value;
		_options[key] = value;
	}

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
	out << _options;

	sav << out.c_str();
	sav.close();
}

/**
 * Returns the game's version in x.x format.
 * @return String with version number.
 */
std::string getVersion()
{
	return _version;
}

/**
 * Returns the game's Data folder where resources
 * and X-Com files are loaded from.
 * @return Full path to Data folder.
 */
std::string getDataFolder()
{
	return _dataFolder;
}

/**
 * Returns the game's User folder where settings
 * and saves are stored in.
 * @return Full path to User folder.
 */
std::string getUserFolder()
{
	return _userFolder;
}

/**
 * Returns an option in string format.
 * @param id Option ID.
 * @return Option value.
 */
std::string getString(const std::string& id)
{
	return _options[id];
}

/**
 * Returns an option in integer format.
 * @param id Option ID.
 * @return Option value.
 */
int getInt(const std::string& id)
{
	std::stringstream ss;
	int value;
	ss << std::dec << _options[id];
	ss >> std::dec >> value;
	return value;
}

/**
 * Returns an option in boolean format.
 * @param id Option ID.
 * @return Option value.
 */
bool getBool(const std::string& id)
{
	std::stringstream ss;
	bool value;
	ss << std::boolalpha << _options[id];
	ss >> std::boolalpha >> value;
	return value;
}

/**
 * Changes an option in string format.
 * @param id Option ID.
 * @param value New option value.
 */
void setString(const std::string& id, const std::string& value)
{
	_options[id] = value;
}

/**
 * Changes an option in integer format.
 * @param id Option ID.
 * @param value New option value.
 */
void setInt(const std::string& id, int value)
{
	std::stringstream ss;
	ss << std::dec << value;
	_options[id] = ss.str();
}

/**
 * Changes an option in boolean format.
 * @param id Option ID.
 * @param value New option value.
 */
void setBool(const std::string& id, bool value)
{
	std::stringstream ss;
	ss << std::boolalpha << value;
	_options[id] = ss.str();
}

}
}
