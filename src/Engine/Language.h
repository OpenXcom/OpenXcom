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
 *e
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef OPENXCOM_LANGUAGE_H
#define OPENXCOM_LANGUAGE_H

#include <map>
#include <vector>
#include <string>
#include <yaml-cpp/yaml.h>
#include "LocalizedText.h"
#include "../Savegame/Soldier.h"

namespace OpenXcom
{
enum TextDirection { DIRECTION_LTR, DIRECTION_RTL };
enum TextWrapping { WRAP_WORDS, WRAP_LETTERS };

class TextList;
class ExtraStrings;
class LanguagePlurality;

/**
 * Contains strings used throughout the game for localization.
 * Languages are just a set of strings identified by an ID string.
 */
class Language
{
private:
	std::string _id;
	std::map<std::string, LocalizedText> _strings;
	LanguagePlurality *_handler;
	TextDirection _direction;
	TextWrapping _wrap;

	static std::map<std::string, std::wstring> _names;
	static std::vector<std::string> _rtl, _cjk;

	/// Parses a text string loaded from an external file.
	std::wstring loadString(const std::string &s) const;
public:
	/// Creates a blank language.
	Language();
	/// Cleans up the language.
	~Language();
	/// Converts a wide-string to UTF-8.
	static std::string wstrToUtf8(const std::wstring& src);
	/// Converts a wide-string to local-codepage string.
	static std::string wstrToCp(const std::wstring &src);
	/// Converts a wide-string to filesystem string.
	static std::string wstrToFs(const std::wstring &src);
	/// Converts a UTF-8 string to wide-string.
	static std::wstring utf8ToWstr(const std::string& src);
	/// Converts a local-codepage string to wide-string.
	static std::wstring cpToWstr(const std::string& src);
	/// Converts a filesystem string to wide-string.
	static std::wstring fsToWstr(const std::string &src);
	/// Replaces a substring.
	static void replace(std::string &str, const std::string &find, const std::string &replace);
	/// Replaces a substring.
	static void replace(std::wstring &str, const std::wstring &find, const std::wstring &replace);
	/// Gets list of languages in the data directory.
	static void getList(std::vector<std::string> &files, std::vector<std::wstring> &names);
	/// Loads the language from a YAML file.
	void load(const std::string &filename);
	/// Loads the language from a ruleset file.
	void load(ExtraStrings *strings);
	/// Gets the language's ID.
	std::string getId() const;
	/// Gets the language's name.
	std::wstring getName() const;
	/// Outputs the language to a HTML file.
	void toHtml(const std::string &filename) const;
	/// Get a localized text.
	const LocalizedText &getString(const std::string &id) const;
	/// Get a quantity-depended localized text.
	LocalizedText getString(const std::string &id, unsigned n) const;
	/// Get a gender-depended localized text.
	const LocalizedText &getString(const std::string &id, SoldierGender gender) const;
	/// Gets the direction of text in this language.
	TextDirection getTextDirection() const;
	/// Gets the wrapping of text in this language.
	TextWrapping getTextWrapping() const;
};

}

#endif
