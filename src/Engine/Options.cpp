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
#include <SDL.h>
#include <SDL_keysym.h>
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

std::string _version = "0.5.0";
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
	_options.clear();
#ifdef DINGOO
	setInt("displayWidth", 320);
	setInt("displayHeight", 200);
	setBool("fullscreen", true);
	setBool("asyncBlit", false);
	setInt("keyboardMode", KEYBOARD_OFF);
#else
	setInt("displayWidth", 640);
	setInt("displayHeight", 400);
	setBool("fullscreen", false);
	setBool("asyncBlit", true);
	setInt("keyboardMode", KEYBOARD_ON);
#endif
	setBool("useScaleFilter", false);
	setBool("useHQXFilter", false);
	setBool("debug", false);
	setBool("debugUi", false);
	setBool("mute", false);
	setInt("soundVolume", MIX_MAX_VOLUME);
	setInt("musicVolume", MIX_MAX_VOLUME);
	setString("language", "");
	setInt("battleScrollSpeed", 24); // 8, 16, 24, 32, 40
	setInt("battleScrollType", SCROLL_AUTO);
	setInt("battleScrollDragButton", SDL_BUTTON_MIDDLE); 
	setBool("battleScrollDragInvert", false); // true drags away from the cursor, false drags towards (like a grab)
	setInt("battleScrollDragTimeTolerance", 300); // miliSecond
	setInt("battleScrollDragPixelTolerance", 10); // count of pixels
	setInt("battleFireSpeed", 20); // 40, 30, 20, 10, 5, 1
	setInt("battleXcomSpeed", 30); // 40, 30, 20, 10, 5, 1
	setInt("battleAlienSpeed", 30); // 40, 30, 20, 10, 5, 1
	setBool("battleInstantGrenade", false); // set to true if you want to play with the alternative grenade handling
	setBool("battlePreviewPath", false); // requires double-click to confirm moves
	setBool("battleRangeBasedAccuracy", false);
	setBool("fpsCounter", false);
	setBool("craftLaunchAlways", false);
	setBool("globeSeasons", false);
	setBool("globeAllRadarsOnBaseBuild", true);
	setInt("audioSampleRate", 22050);
	setInt("audioBitDepth", 16);
	setInt("pauseMode", 0);
	setBool("customInitialBase", false);
	setBool("aggressiveRetaliation", false);
	setBool("strafe", false);
	setBool("battleNotifyDeath", false);
	setBool("allowBuildingQueue", false);
	setBool("allowAutoSellProduction", false);
	setBool("showFundsOnGeoscape", false);
	setBool("showMoreStatsInInventoryView", false);
	// controls
	setInt("keyOk", SDLK_RETURN);
	setInt("keyCancel", SDLK_ESCAPE);
	setInt("keyScreenshot", SDLK_F12);
	setInt("keyFps", SDLK_F5);
	setInt("keyGeoLeft", SDLK_LEFT);
	setInt("keyGeoRight", SDLK_RIGHT);
	setInt("keyGeoUp", SDLK_UP);
	setInt("keyGeoDown", SDLK_DOWN);
	setInt("keyGeoZoomIn", SDLK_PLUS);
	setInt("keyGeoZoomOut", SDLK_MINUS);
	setInt("keyGeoSpeed1", SDLK_1);
	setInt("keyGeoSpeed2", SDLK_2);
	setInt("keyGeoSpeed3", SDLK_3);
	setInt("keyGeoSpeed4", SDLK_4);
	setInt("keyGeoSpeed5", SDLK_5);
	setInt("keyGeoSpeed6", SDLK_6);
	setInt("keyGeoIntercept", SDLK_i);
	setInt("keyGeoBases", SDLK_b);
	setInt("keyGeoGraphs", SDLK_g);
	setInt("keyGeoUfopedia", SDLK_u);
	setInt("keyGeoOptions", SDLK_ESCAPE);
	setInt("keyGeoFunding", SDLK_f);
	setInt("keyGeoToggleDetail", SDLK_TAB);
	setInt("keyGeoToggleRadar", SDLK_r);
	setInt("keyBattleLeft", SDLK_LEFT);
	setInt("keyBattleRight", SDLK_RIGHT);
	setInt("keyBattleUp", SDLK_UP);
	setInt("keyBattleDown", SDLK_DOWN);
	setInt("keyBattleLevelUp", SDLK_PAGEUP);
	setInt("keyBattleLevelDown", SDLK_PAGEDOWN);
	setInt("keyBattleCenterUnit", SDLK_HOME);
	setInt("keyBattlePrevUnit", SDLK_LSHIFT);
	setInt("keyBattleNextUnit", SDLK_TAB);
	setInt("keyBattleDeselectUnit", SDLK_BACKSLASH);
	setInt("keyBattleInventory", SDLK_i);
	setInt("keyBattleMap", SDLK_m);
	setInt("keyBattleOptions", SDLK_ESCAPE);
	setInt("keyBattleEndTurn", SDLK_BACKSPACE);
	setInt("keyBattleAbort", SDLK_a);
	setInt("keyBattleStats", SDLK_F1);
	setInt("keyBattleKneel", SDLK_k);
	setInt("keyBattleReload", SDLK_r);
	setInt("keyBattlePersonalLighting", SDLK_l);
	setInt("keyBattleReserveNone", SDLK_F2);
	setInt("keyBattleReserveSnap", SDLK_F3);
	setInt("keyBattleReserveAimed", SDLK_F4);
	setInt("keyBattleReserveAuto", SDLK_F5);
	setInt("keyBattleCenterEnemy1", SDLK_1);
	setInt("keyBattleCenterEnemy2", SDLK_2);
	setInt("keyBattleCenterEnemy3", SDLK_3);
	setInt("keyBattleCenterEnemy4", SDLK_4);
	setInt("keyBattleCenterEnemy5", SDLK_5);
	setInt("keyBattleCenterEnemy6", SDLK_6);
	setInt("keyBattleCenterEnemy7", SDLK_7);
	setInt("keyBattleCenterEnemy8", SDLK_8);
	setInt("keyBattleCenterEnemy9", SDLK_9);

	_rulesets.clear();
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
		//throw Exception(filename + ".cfg" + "not found");
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
		Log(LOG_WARNING) << "Failed to save " << filename << ".cfg";
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
