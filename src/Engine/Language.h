/*
 * Copyright 2010 Daniel Albano
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

#include <vector>
#include <string>
#include "../Resource/LangString.h"

/**
 * Contains std::strings used throughout the game for localization.
 * Languages are just a set of std::strings identified by an ID number.
 */
class Language
{
private:
	std::vector<std::string> _strings;
public:
	/// Creates a blank language.
	Language();
	/// Cleans up the language.
	~Language();
	/// Loads an X-Com language file.
	void loadDat(std::string filename);
	/// Gets a std::string from the language.
	std::string getString(LangString id);
};

#endif
