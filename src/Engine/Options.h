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
#ifndef OPENXCOM_OPTIONS_H
#define OPENXCOM_OPTIONS_H

#include <string>

namespace OpenXcom
{

/**
 * Container for all the various global game options
 * and customizable settings.
 */
namespace Options
{
	/// Creates a new set of options.
	void create();
	/// Loads options from command line arguments.
	void loadArgs(int argc, char** args);
	/// Loads options from YAML.
	void load(const std::string &filename = "options");
	/// Saves options to YAML.
	void save(const std::string &filename = "options");
	/// Gets the current game version.
	std::string getVersion();
	/// Gets the game's data folder.
	std::string getDataFolder();
	/// Sets the game's data folder.
	void setDataFolder(const std::string& folder);
	/// Gets the game's user folder.
	std::string getUserFolder();
	/// Sets the game's user folder.
	void setUserFolder(const std::string& folder);
	/// Gets a string option.
	std::string getString(const std::string& id);
	/// Gets an integer option.
	int getInt(const std::string& id);
	/// Gets a boolean option.
	bool getBool(const std::string& id);
	/// Sets a string option.
	void setString(const std::string& id, const std::string& value);
	/// Sets an integer option.
	void setInt(const std::string& id, int value);
	/// Sets a boolean option.
	void setBool(const std::string& id, bool value);
}

}

#endif
