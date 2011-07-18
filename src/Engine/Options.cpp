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

namespace OpenXcom
{
namespace Options
{

std::string _dataFolder = "./DATA/";
std::string _userFolder = "./USER/";
std::map<std::string, std::string> _options;

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

std::string setString(std::string id, std::string value)
{
	_options[id] = value;
}

std::string setInt(std::string id, int value)
{
	std::stringstream ss;
	ss << std::dec << value;
	_options[id] = ss.str();
}

std::string setBool(std::string id, bool value)
{
	std::stringstream ss;
	ss << std::boolalpha << value;
	_options[id] = ss.str();
}

}
}
