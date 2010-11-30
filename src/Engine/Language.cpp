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
#include "Language.h"
#include <iostream>
#include <fstream>

/**
 * Initializes an empty language file.
 */
Language::Language() : _name(""), _strings()
{

}

/**
 *
 */
Language::~Language()
{
	
}

/**
 * Loads pairs of null-terminated strings contained in
 * a raw text file into the Language. Each pair is made of
 * an ID and a localized string. For duplicate strings, _UC denotes
 * the uppercase string and __ denotes a linebreak in the string.
 * @param filename Filename of the LNG file.
 */
void Language::loadLng(const std::string &filename)
{
	_strings.clear();
	
	// Load file and put text in map
	std::ifstream txtFile (filename.c_str(), std::ios::in | std::ios::binary);
	if (!txtFile)
	{
		throw "Failed to load LNG";
	}
	
	char value;
	std::string buffer, bufid, bufstr;
	bool first = true, id = true;

	while (txtFile.read(&value, 1))
	{
		if (value != '\0')
		{
			buffer += value;
		}
		else
		{
			// Get language name
			if (first)
			{
				_name = buffer;
				first = false;
			}
			else
			{
				// Get ID
				if (id)
				{
					bufid = buffer;
				}
				// Get string
				else
				{
					bufstr = buffer;
					_strings[bufid] = bufstr;
				}
				id = !id;
			}
			buffer.clear();
		}
	}

	if (!txtFile.eof())
		throw "Invalid data from file";

	txtFile.close();
}

/**
 * Returns the language's name in its native language.
 * @return Language name.
 */
std::string Language::getName() const
{
	return _name;
}

/**
 * Returns the localizable string with the specified ID.
 * If it's not found, just returns the ID.
 * @param id ID of the string.
 * @return String with the requested ID.
 */
std::string Language::getString(const std::string &id) const
{
	std::map<std::string, std::string>::const_iterator s = _strings.find(id);
	if (s == _strings.end())
	{
		std::cout << "WARNING: " << id << " not found in " << _name << std::endl;
		return id;
	}
	else
	{
		return s->second;
	}
}
