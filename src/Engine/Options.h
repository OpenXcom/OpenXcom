/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#ifndef OPENXCOM_OPTIONS_H
#define OPENXCOM_OPTIONS_H

#include <SDL.h>
#include <string>
#include <vector>
#include "OptionInfo.h"
#include "ModInfo.h"

namespace OpenXcom
{

/// Battlescape drag scrolling types.
enum ScrollType { SCROLL_NONE, SCROLL_TRIGGER, SCROLL_AUTO };
/// Keyboard input modes.
enum KeyboardType { KEYBOARD_OFF, KEYBOARD_ON, KEYBOARD_VIRTUAL };
/// Savegame sorting modes.
enum SaveSort { SORT_NAME_ASC, SORT_NAME_DESC, SORT_DATE_ASC, SORT_DATE_DESC };
/// Music format preferences.
enum MusicFormat { MUSIC_AUTO, MUSIC_FLAC, MUSIC_OGG, MUSIC_MP3, MUSIC_MOD, MUSIC_WAV, MUSIC_ADLIB, MUSIC_MIDI };
/// Sound format preferences.
enum SoundFormat { SOUND_AUTO, SOUND_14, SOUND_10 };
/// Video format preferences.
enum VideoFormat { VIDEO_FMV, VIDEO_SLIDE };
/// Path preview modes (can be OR'd together).
enum PathPreview {
	PATH_NONE    = 0x00, // 0000 (must always be zero)
	PATH_ARROWS  = 0x01, // 0001
	PATH_TU_COST = 0x02, // 0010
	PATH_FULL    = 0x03  // 0011 (must always be all values combined)
};

enum ScaleType
{
	SCALE_ORIGINAL,
	SCALE_15X,
	SCALE_2X,
	SCALE_SCREEN_DIV_3,
	SCALE_SCREEN_DIV_2,
	SCALE_SCREEN
};
/**
 * Container for all the various global game options
 * and customizable settings.
 */
namespace Options
{
#define OPT extern
#include "Options.inc.h"
#undef OPT

	/// Creates the options info.
	void create();
	/// Restores default options.
	void resetDefault();
	/// Initializes the options settings.
	bool init(int argc, char *argv[]);
	/// Loads options from YAML.
	void load(const std::string &filename = "options");
	/// Saves options to YAML.
	void save(const std::string &filename = "options");
	/// Gets the game's data folder.
	std::string getDataFolder();
	/// Sets the game's data folder.
	void setDataFolder(const std::string &folder);
	/// Gets the game's data list.
	const std::vector<std::string> &getDataList();
	/// Gets the game's user folder.
	std::string getUserFolder();
	/// Gets the game's config folder.
	std::string getConfigFolder();
	/// Gets the game's master mod user folder.
	std::string getMasterUserFolder();
	/// Gets the game's options.
	const std::vector<OptionInfo> &getOptionInfo();
	/// Sets the game's data, user and config folders.
	void setFolders();
	/// Sets the game's user master folders.
	void userSplitMasters();
	/// Update game options from config file and command line.
	void updateOptions();
	/// Backup display options.
	void backupDisplay();
	/// Switches display options.
	void switchDisplay();
	/// returns the id of the active master mod
	std::string getActiveMaster();
	/// Maps resources in active mods to the virtual file system
	void mapResources();
	/// Gets the map of mod ids to mod infos
	const std::map<std::string, ModInfo> &getModInfos();
}

}

#endif
