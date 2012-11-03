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
 *e
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef OPENXCOM_LANGUAGE_H
#define OPENXCOM_LANGUAGE_H

#include <map>
#include <vector>
#include <string>

namespace OpenXcom
{

class TextList;

/**
 * Contains strings used throughout the game for localization.
 * Languages are just a set of strings identified by an ID string.
 */
class Language
{
private:
	std::wstring _name;
	std::map<std::string, std::wstring> _strings;
public:
	/// Creates a blank language.
	Language();
	/// Cleans up the language.
	~Language();
	/// Converts a wide-string to UTF-8.
	static std::string wstrToUtf8(const std::wstring& src);
	/// Converts a UTF-8 string to wide-string.
	static std::wstring utf8ToWstr(const std::string& src);
	/// Converts a local-codepage string to wide-string.
	static std::wstring cpToWstr(const std::string& src);
	/// Replaces a substring.
	static void replace(std::string &str, const std::string &find, const std::string &replace);
	/// Gets list of languages in the data directory.
	static std::vector<std::string> getList(TextList *list);
	/// Loads an OpenXcom language file.
	void loadLng(const std::string &filename);
	/// Gets the language's name.
	std::wstring getName() const;
	/// Gets a string from the language.
	std::wstring getString(const std::string &id) const;
	/// Outputs the language to a HTML file.
	void toHtml(const std::string &filename) const;
};

}

#endif
