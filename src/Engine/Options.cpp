/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include <SDL_mixer.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <yaml-cpp/yaml.h>
#include "Exception.h"
#include "Logger.h"
#include "CrossPlatform.h"

namespace OpenXcom
{
namespace Options
{

std::string _version = "0.4.5";
std::string _dataFolder = "";
std::vector<std::string> _dataList;
std::string _userFolder = "";
std::string _configFolder = "";
std::vector<std::string> _userList;
std::map<std::string, std::string> _options;
std::vector<std::string> _rulesets;

/**
 * Creates a default set of options based on the system.
 */
void createDefault()
{
#ifdef DINGOO
	setInt("displayWidth", 320);
	setInt("displayHeight", 200);
	setBool("fullscreen", true);
	setInt("keyboardMode", KEYBOARD_OFF);
#else
	setInt("displayWidth", 640);
	setInt("displayHeight", 400);
	setBool("fullscreen", false);
	setInt("keyboardMode", KEYBOARD_ON);
#endif
	setBool("debug", false);
	setBool("debugUi", false);
	setBool("mute", false);
	setInt("soundVolume", MIX_MAX_VOLUME);
	setInt("musicVolume", MIX_MAX_VOLUME);
	setString("language", "");
	setInt("battleScrollSpeed", 24); // 8, 16, 24, 32, 40
	setInt("battleScrollType", SCROLL_AUTO);
	setString("battleScrollButton", "RMB"); // RMB, MMB, None  (Right-Mouse-Button, Middle-Mouse-Button, None)
	setString("battleScrollButtonInvertMode", "Normal"); // Normal, Inverted
	setInt("battleScrollButtonTimeTolerancy", 300); // miliSecond
	setInt("battleScrollButtonPixelTolerancy", 10); // count of pixels
	setInt("battleFireSpeed", 20); // 30, 25, 20, 15, 10, 5
	setInt("battleXcomSpeed", 40); // 60, 50, 40, 30, 20, 10
	setInt("battleAlienSpeed", 40); // 60, 50, 40, 30, 20, 10
	setBool("battleAltGrenade", false); // set to true if you want to play with the alternative grenade handling
	setBool("battlePreviewPath", false);
	setBool("battleRangeBasedAccuracy", false);
	setBool("fpsCounter", false);
	setBool("craftLaunchAlways", false);
	setBool("globeSeasons", false);
	setInt("audioSampleRate", 22050);
	setInt("audioBitDepth", 16);
	setInt("pauseMode", 0);
	setBool("customInitialBase", false);

	_rulesets.push_back("Xcom1Ruleset");
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
		std::string arg = args[i];
		if ((arg[0] == '-' || arg[0] == '/') && arg.length() > 1)
		{
			std::string argname;
			if (arg[1] == '-' && arg.length() > 2)
				argname = arg.substr(2, arg.length()-1);
			else
				argname = arg.substr(1, arg.length()-1);
			std::transform(argname.begin(), argname.end(), argname.begin(), ::tolower);
			if (argc > i + 1)
			{
				std::map<std::string, std::string>::iterator it = _options.find(argname);
				if (it != _options.end())
				{
					it->second = args[i+1];
				}
				else if (argname == "data")
				{
					_dataFolder = CrossPlatform::endPath(args[i+1]);
				}
				else if (argname == "user")
				{
					_userFolder = CrossPlatform::endPath(args[i+1]);
				}
				else
				{
					Log(LOG_WARNING) << "Unknown option: " << argname;
				}
			}
			else
			{
				Log(LOG_WARNING) << "Unknown option: " << argname;
			}
		}
	}
	if (_userFolder != "")
	{
		load();
	}
}

/*
 * Displays command-line help when appropriate.
 * @param argc Number of arguments.
 * @param args Array of argument strings.
 */
bool showHelp(int argc, char** args)
{
	std::stringstream help;
	help << "OpenXcom v" << Options::getVersion() << std::endl;
	help << "Usage: openxcom [OPTION]..." << std::endl << std::endl;
	help << "-data PATH" << std::endl;
	help << "        use PATH as the default Data Folder instead of auto-detecting" << std::endl << std::endl;
	help << "-user PATH" << std::endl;
	help << "        use PATH as the default User Folder instead of auto-detecting" << std::endl << std::endl;
	help << "-KEY VALUE" << std::endl;
	help << "        set option KEY to VALUE instead of default/loaded value (eg. -displayWidth 640)" << std::endl << std::endl;
	help << "-help" << std::endl;
	help << "-?" << std::endl;
	help << "        show command-line help" << std::endl;
	for (int i = 1; i < argc; ++i)
	{
		std::string arg = args[i];
		if ((arg[0] == '-' || arg[0] == '/') && arg.length() > 1)
		{
			std::string argname;
			if (arg[1] == '-' && arg.length() > 2)
				argname = arg.substr(2, arg.length()-1);
			else
				argname = arg.substr(1, arg.length()-1);
			std::transform(argname.begin(), argname.end(), argname.begin(), ::tolower);
			if (argname == "help" || argname == "?")
			{
				std::cout << help.str();
				return true;
			}
		}
	}
	return false;
}

/**
 * Handles the initialization of setting up default options
 * and finding and loading any existing ones.
 * @param argc Number of arguments.
 * @param args Array of argument strings.
 */
bool init(int argc, char** args)
{
	if (showHelp(argc, args))
		return false;
	createDefault();
	loadArgs(argc, args);
	if (_dataFolder == "")
	{
		_dataList = CrossPlatform::findDataFolders();
		// Missing data folder is handled in StartState
	}
	if (_userFolder == "")
	{
		std::vector<std::string> user = CrossPlatform::findUserFolders();
		_configFolder = CrossPlatform::findConfigFolder();

		// Look for an existing user folder
		for (std::vector<std::string>::iterator i = user.begin(); i != user.end(); ++i)
		{
			if (CrossPlatform::folderExists(*i))
			{
				_userFolder = *i;
				break;
			}
		}

		// Set up folders
		if (_userFolder == "")
		{
			for (std::vector<std::string>::iterator i = user.begin(); i != user.end(); ++i)
			{
				if (CrossPlatform::createFolder(*i))
				{
					_userFolder = *i;
					break;
				}
			}
		}
		if (_configFolder == "")
		{
			_configFolder = _userFolder;
		}

		// Load existing options
		if (CrossPlatform::folderExists(_configFolder))
		{
			load();
		}
		// Create config folder and save options
		else
		{
			CrossPlatform::createFolder(_configFolder);
			save();
		}
	}
	std::string s = getUserFolder();
	s += "openxcom.log";
	Logger::logFile() = s;
	FILE *file = fopen(Logger::logFile().c_str(), "w");
    fflush(file);
	fclose(file);
	Log(LOG_INFO) << "Data folder is: " << _dataFolder;
	for (std::vector<std::string>::iterator i = _dataList.begin(); i != _dataList.end(); ++i)
	{
		Log(LOG_INFO) << *i;
	}
	Log(LOG_INFO) << "User folder is: " << _userFolder;
	Log(LOG_INFO) << "Config folder is: " << _configFolder;
	Log(LOG_INFO) << "Options loaded successfully.";
	return true;
}

/**
 * Loads options from a YAML file.
 * @param filename YAML filename.
 */
void load(const std::string &filename)
{
	std::string s = _configFolder + filename + ".cfg";
	std::ifstream fin(s.c_str());
	if (!fin)
	{
		//throw Exception("Failed to load options");
		return;
	}
	YAML::Parser parser(fin);
	YAML::Node doc;

	parser.GetNextDocument(doc);
	const YAML::Node *options = doc.FindValue("options");
	if (!options)
	{
		options = &doc;
	}

	for (YAML::Iterator i = options->begin(); i != options->end(); ++i)
	{
		std::string key, value;
		i.first() >> key;
		i.second() >> value;
		_options[key] = value;
	}

	if (const YAML::Node *pName = doc.FindValue("rulesets"))
	{
		(*pName) >> _rulesets;
	}

	fin.close();
}

/**
 * Saves options to a YAML file.
 * @param filename YAML filename.
 */
void save(const std::string &filename)
{
	std::string s = _configFolder + filename + ".cfg";
	std::ofstream sav(s.c_str());
	if (!sav)
	{
		//throw Exception("Failed to save options");
		return;
	}
	YAML::Emitter out;

	out << YAML::BeginMap;
	out << YAML::Key << "options" << YAML::Value << _options;
	out << YAML::Key << "rulesets" << YAML::Value << _rulesets;
	out << YAML::EndMap;

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
 * Returns the game's current Data folder where resources
 * and X-Com files are loaded from.
 * @return Full path to Data folder.
 */
std::string getDataFolder()
{
	return _dataFolder;
}

/**
 * Changes the game's current Data folder where resources
 * and X-Com files are loaded from.
 * @param folder Full path to Data folder.
 */
void setDataFolder(const std::string &folder)
{
	_dataFolder = folder;
}

/**
 * Returns the game's list of possible Data folders.
 * @return List of Data paths.
 */
std::vector<std::string> *getDataList()
{
	return &_dataList;
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

/**
 * Returns the list of rulesets to be used by the game.
 * @return Ruleset list.
 */
std::vector<std::string> getRulesets()
{
	return _rulesets;
}

}
}
