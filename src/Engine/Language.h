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
#ifndef OPENXCOM_LANGUAGE_H
#define OPENXCOM_LANGUAGE_H

#include <map>
#include <string>

namespace OpenXcom
{

/**
 * Contains strings used throughout the game for localization.
 * Languages are just a set of strings identified by an ID string.
 */
class Language
{
private:
	std::string _name;
	std::map<std::string, std::string> _strings;
public:
	/// Creates a blank language.
	Language();
	/// Cleans up the language.
	~Language();
	/// Loads an OpenXcom language file.
	void loadLng(const std::string &filename);
	/// Gets the language's name.
	std::string getName() const;
	/// Gets a string from the language.
	std::string getString(const std::string &id) const;
	/// Outputs the language to a HTML file.
	void toHtml() const;
};

}

#endif
