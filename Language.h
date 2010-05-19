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
#ifndef OPENXCOM__LANGUAGE_H
#define OPENXCOM__LANGUAGE_H

#include "LangString.h"
#include <vector>
#include <string>
#include <fstream>

using namespace std;

/**
 * Contains strings used throughout the game for localization.
 * Languages are just a set of strings identified by an ID number.
 */
class Language
{
private:
	vector<string> _strings;
public:
	/// Creates a blank language.
	Language();
	/// Cleans up the language.
	~Language();
	/// Loads an X-Com language file.
	void loadDat(string filename);
	/// Gets a string from the language.
	string getString(LangString id);
};

#endif