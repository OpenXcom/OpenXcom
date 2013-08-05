/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include "../version.h"
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

std::string _dataFolder = "";
std::vector<std::string> _dataList;
std::string _userFolder = "";
std::string _configFolder = "";
std::vector<std::string> _userList;
std::map<std::string, std::string> _options, _commandLineOptions;
std::vector<std::string> _rulesets;
std::vector<std::string> _purchaseexclusions;

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
	setBool("anytimePsiTraining", false);
	setBool("playIntro", true);
	setInt("maxFrameSkip", 8);
	setBool("traceAI", false);
	setBool("sneakyAI", false);
	setBool("weaponSelfDestruction", false);
	setBool("researchedItemsWillSpent", false);
	setInt("baseXResolution", 320);
	setInt("baseYResolution", 200);
	setBool("useScaleFilter", false);
	setBool("useHQXFilter", false);
	setBool("useOpenGL", false);
	setBool("checkOpenGLErrors", false);
	setString("useOpenGLShader", "Shaders/Openxcom.OpenGL.shader");
	setBool("vSyncForOpenGL", true);
	setBool("useOpenGLSmoothing", true);
	setBool("debug", false);
	setBool("debugUi", false);
	setBool("mute", false);
	setInt("soundVolume", MIX_MAX_VOLUME);
	setInt("musicVolume", MIX_MAX_VOLUME);
	setString("language", "");
	setInt("battleScrollSpeed", 12); // 4, 8, 12, 16, 24
	setInt("battleScrollType", SCROLL_AUTO);
	setInt("battleScrollDragButton", SDL_BUTTON_MIDDLE); 
	setBool("battleScrollDragInvert", false); // true drags away from the cursor, false drags towards (like a grab)
	setInt("battleScrollDragTimeTolerance", 300); // miliSecond
	setInt("battleScrollDragPixelTolerance", 10); // count of pixels
	setInt("battleFireSpeed", 20); // 40, 30, 20, 10, 5, 1
	setInt("battleXcomSpeed", 30); // 40, 30, 20, 10, 5, 1
	setInt("battleAlienSpeed", 30); // 40, 30, 20, 10, 5, 1
	setBool("battleInstantGrenade", false); // set to true if you want to play with the alternative grenade handling
	setInt("battleExplosionHeight", 0); //0, 1, 2, 3
	setInt("battleNewPreviewPath", 0); // requires double-click to confirm moves 0 = none, 1 = arrows, 2 = numbers, 3 = full
	setBool("battleRangeBasedAccuracy", false);
	setBool("fpsCounter", false);
	setBool("craftLaunchAlways", false);
	setBool("globeSeasons", false);
	setBool("globeAllRadarsOnBaseBuild", true);
	setBool("allowChangeListValuesByMouseWheel", false); // It applies only for lists, not for scientists/engineers screen
	setInt("autosave", 0);
	setInt("changeValueByMouseWheel", 10);
	setInt("audioSampleRate", 22050);
	setInt("audioBitDepth", 16);
	setInt("pauseMode", 0);
	setBool("alienContainmentLimitEnforced", false);
	setBool("canSellLiveAliens", false);
	setBool("canTransferCraftsWhileAirborne", false); // When the craft can reach the destination base with its fuel
	setBool("canManufactureMoreItemsPerHour", false);
	setBool("customInitialBase", false);
	setBool("aggressiveRetaliation", false);
	setBool("strafe", false);
	setBool("battleNotifyDeath", false);
	setBool("allowBuildingQueue", false);
	setBool("allowAutoSellProduction", false);
	setBool("showFundsOnGeoscape", false);
	setBool("showMoreStatsInInventoryView", false);
	setBool("allowResize", false);
	setInt("windowedModePositionX", -1);
	setInt("windowedModePositionY", -1);
	setBool("battleAutoEnd", false);
	setBool("allowPsionicCapture", false);
	setBool("borderless", false);
	setBool("captureMouse", false);

	// new battle mode data
	setInt("NewBattleMission", 0);
	setInt("NewBattleTerrain", 0);
	setInt("NewBattleItemLevel", 0);
	setInt("NewBattleAlienRace", 0);
	setInt("NewBattleDifficulty", 0);
	setInt("NewBattleDarkness", 0);
	setInt("NewBattleCraft", 0);
	
	// new battle loadout data
	setInt("NewBattle_STR_AC_AP_AMMO", 0);
	setInt("NewBattle_STR_AC_HE_AMMO", 0);
	setInt("NewBattle_STR_AC_I_AMMO", 0);
	setInt("NewBattle_STR_ALIEN_GRENADE", 0);
	setInt("NewBattle_STR_AUTO_CANNON", 0);
	setInt("NewBattle_STR_BLASTER_BOMB", 0);
	setInt("NewBattle_STR_BLASTER_LAUNCHER", 0);
	setInt("NewBattle_STR_ELECTRO_FLARE", 0);
	setInt("NewBattle_STR_GRENADE", 0);
	setInt("NewBattle_STR_HC_AP_AMMO", 0);
	setInt("NewBattle_STR_HC_HE_AMMO", 0);
	setInt("NewBattle_STR_HC_I_AMMO", 0);
	setInt("NewBattle_STR_HEAVY_CANNON", 0);
	setInt("NewBattle_STR_HEAVY_LASER", 0);
	setInt("NewBattle_STR_HEAVY_PLASMA", 0);
	setInt("NewBattle_STR_HEAVY_PLASMA_CLIP", 0);
	setInt("NewBattle_STR_HIGH_EXPLOSIVE", 0);
	setInt("NewBattle_STR_INCENDIARY_ROCKET", 0);
	setInt("NewBattle_STR_LARGE_ROCKET", 0);
	setInt("NewBattle_STR_LASER_PISTOL", 0);
	setInt("NewBattle_STR_LASER_RIFLE", 0);
	setInt("NewBattle_STR_MEDI_KIT", 0);
	setInt("NewBattle_STR_MIND_PROBE", 0);
	setInt("NewBattle_STR_MOTION_SCANNER", 0);
	setInt("NewBattle_STR_PISTOL", 0);
	setInt("NewBattle_STR_PISTOL_CLIP", 0);
	setInt("NewBattle_STR_PLASMA_PISTOL", 0);
	setInt("NewBattle_STR_PLASMA_PISTOL_CLIP", 0);
	setInt("NewBattle_STR_PLASMA_RIFLE", 0);
	setInt("NewBattle_STR_PLASMA_RIFLE_CLIP", 0);
	setInt("NewBattle_STR_PROXIMITY_GRENADE", 0);
	setInt("NewBattle_STR_PSI_AMP", 0);
	setInt("NewBattle_STR_RIFLE", 0);
	setInt("NewBattle_STR_RIFLE_CLIP", 0);
	setInt("NewBattle_STR_ROCKET_LAUNCHER", 0);
	setInt("NewBattle_STR_SMALL_LAUNCHER", 0);
	setInt("NewBattle_STR_SMALL_ROCKET", 0);
	setInt("NewBattle_STR_SMOKE_GRENADE", 0);
	setInt("NewBattle_STR_STUN_BOMB", 0);
	setInt("NewBattle_STR_STUN_ROD", 0);

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
	setInt("keyQuickSave", SDLK_F6);
	setInt("keyQuickLoad", SDLK_F9);
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
	setInt("keyBattleStats", SDLK_s);
	setInt("keyBattleKneel", SDLK_k);
	setInt("keyBattleReserveKneel", SDLK_j);
	setInt("keyBattleReload", SDLK_r);
	setInt("keyBattlePersonalLighting", SDLK_l);
	setInt("keyBattleReserveNone", SDLK_F1);
	setInt("keyBattleReserveSnap", SDLK_F2);
	setInt("keyBattleReserveAimed", SDLK_F3);
	setInt("keyBattleReserveAuto", SDLK_F4);
	setInt("keyBattleCenterEnemy1", SDLK_1);
	setInt("keyBattleCenterEnemy2", SDLK_2);
	setInt("keyBattleCenterEnemy3", SDLK_3);
	setInt("keyBattleCenterEnemy4", SDLK_4);
	setInt("keyBattleCenterEnemy5", SDLK_5);
	setInt("keyBattleCenterEnemy6", SDLK_6);
	setInt("keyBattleCenterEnemy7", SDLK_7);
	setInt("keyBattleCenterEnemy8", SDLK_8);
	setInt("keyBattleCenterEnemy9", SDLK_9);
	setInt("keyBattleCenterEnemy10", SDLK_0);
	setInt("keyBattleVoxelView", SDLK_F10);

	setInt("keyBattleZeroTUs", SDLK_DELETE);

#ifdef __MORPHOS__
	setInt("FPS", 15);
#endif

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
				if (argname == "data")
				{
					_dataFolder = CrossPlatform::endPath(args[i+1]);
				}
				else if (argname == "user")
				{
					_userFolder = CrossPlatform::endPath(args[i+1]);
				}
				else
				{
					// case insensitive lookup of the argument
					bool found = false;
					for(std::map<std::string, std::string>::iterator it = _options.begin(); it != _options.end(); ++it)
					{
 						std::string option = it->first;
						std::transform(option.begin(), option.end(), option.begin(), ::tolower);
						if (option == argname)
						{
							//save this command line option for now, we will apply it later
							_commandLineOptions[it->first]= args[i+1];
							found = true;
							break;
						}
					}
					if(!found)
					{
						Log(LOG_WARNING) << "Unknown option: " << argname;
					}
				}
			}
			else
			{
				Log(LOG_WARNING) << "Unknown option: " << argname;
			}
		}
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
	help << "OpenXcom v" << OPENXCOM_VERSION_SHORT << std::endl;
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
 * @return Was initialized.
 */
bool init(int argc, char** args)
{
	if (showHelp(argc, args))
		return false;
	createDefault();
	loadArgs(argc, args);
	setFolders();
	updateOptions();

	std::string s = getUserFolder();
	s += "openxcom.log";
	Logger::logFile() = s;
	FILE *file = fopen(Logger::logFile().c_str(), "w");
	if(!file)
	{
		std::stringstream error;
		error << "Error: invalid User Folder " << _userFolder << std::endl;
		std::cout << error.str();
		return false;
	}
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
 * Sets up the game's Data folder where the data files
 * are loaded from and the User folder and Config
 * folder where settings and saves are stored in.
 */
void setFolders()
{
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
	}

	if (_configFolder == "")
	{
		_configFolder = _userFolder;
	}
}

/**
 * Updates the game's options with those in the configuation
 * file, if it exists yet, and any supplied on the command line.
 */
void updateOptions()
{
	// Load existing options
	if (CrossPlatform::folderExists(_configFolder))
	{
		try
		{
			load();
		}
		catch (YAML::Exception &e)
		{
			Log(LOG_ERROR) << e.what();
		}
	}
	// Create config folder and save options
	else
	{
		CrossPlatform::createFolder(_configFolder);
		save();
	}

    // now apply options set on the command line, overriding defaults and those loaded from config file
    for(std::map<std::string, std::string>::const_iterator it = _commandLineOptions.begin(); it != _commandLineOptions.end(); ++it)
    {
        _options[it->first] = it->second;
    }
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

	if (const YAML::Node *pName = doc.FindValue("purchaseexclusions"))
	{
		(*pName) >> _purchaseexclusions;
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
	int value = 0;
	if (_options.find(id) != _options.end())
	{
		ss << std::dec << _options[id];
		ss >> std::dec >> value;
	}
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

std::vector<std::string> getPurchaseExclusions()
{
	return _purchaseexclusions;
}

}
}
