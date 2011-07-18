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
	void load();
	void save();
	std::string getDataFolder();
	void setDataFolder(std::string folder);
	std::string getUserFolder();
	void setUserFolder(std::string folder);
	std::string getString(std::string id);
	int getInt(std::string id);
	bool getBool(std::string id);
	void setString(std::string id, std::string value);
	void setInt(std::string id, int value);
	void setBool(std::string id, bool value);
}

}

#endif
