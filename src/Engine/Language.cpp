/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include <assert.h>
#include <locale>
#include <fstream>
#include <cassert>
#include "CrossPlatform.h"
#include "Logger.h"
#include "Exception.h"
#include "Options.h"
#include "LocalizedText.h"
#include "LanguagePlurality.h"
#include "../Ruleset/ExtraStrings.h"
#include "../Interface/TextList.h"
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace OpenXcom
{

std::map<std::string, std::wstring> Language::_names;

/**
 * Initializes an empty language file.
 */
Language::Language() : _id(""), _strings(), _handler(0)
{
	// maps don't have initializers :(
	if (_names.empty())
	{
		_names["en-US"] = utf8ToWstr("English (US)");
		_names["en-GB"] = utf8ToWstr("English (UK)");
		_names["bg-BG"] = utf8ToWstr("Български");
		_names["cs-CZ"] = utf8ToWstr("Česky");
		_names["da"] = utf8ToWstr("Dansk");
		_names["de"] = utf8ToWstr("Deutsch");
		_names["es"] = utf8ToWstr("Español (ES)");
		_names["es-419"] = utf8ToWstr("Español (AL)");
		_names["fr"] = utf8ToWstr("Français");
		_names["fi"] = utf8ToWstr("Suomi");
		_names["grk"] = utf8ToWstr("Ελληνικά");
		_names["hu-HU"] = utf8ToWstr("Magyar");
		_names["it"] = utf8ToWstr("Italiano");
		_names["nl"] = utf8ToWstr("Nederlands");
		_names["no"] = utf8ToWstr("Norsk");
		_names["pl-PL"] = utf8ToWstr("Polski");
		_names["pt-PT"] = utf8ToWstr("Português (PT)");
		_names["ro"] = utf8ToWstr("Română");
		_names["ru"] = utf8ToWstr("Русский");
		_names["sv"] = utf8ToWstr("Svenska");
		_names["tr-TR"] = utf8ToWstr("Türkçe");
		_names["uk"] = utf8ToWstr("Українська");
	}
}

/**
 *
 */
Language::~Language()
{
	delete _handler;
}

/**
 * Takes a wide-character string and converts it
 * to a 8-bit string encoded in UTF-8.
 * @note Adapted from http://stackoverflow.com/questions/148403/utf8-to-from-wide-char-conversion-in-stl
 * @param src Wide-character string.
 * @return UTF-8 string.
 */
std::string Language::wstrToUtf8(const std::wstring& src)
{
	if (src.empty())
		return "";
#ifdef _WIN32
	int size = WideCharToMultiByte(CP_UTF8, 0, &src[0], (int)src.size(), NULL, 0, NULL, NULL);
    std::string str(size, 0);
	WideCharToMultiByte(CP_UTF8, 0, &src[0], (int)src.size(), &str[0], size, NULL, NULL);
	return str;
#else
	std::string out;
    unsigned int codepoint = 0;
    for (std::wstring::const_iterator i = src.begin(); i != src.end(); ++i)
    {
		wchar_t ch = *i;
        if (ch >= 0xd800 && ch <= 0xdbff)
            codepoint = ((ch - 0xd800) << 10) + 0x10000;
        else
        {
            if (ch >= 0xdc00 && ch <= 0xdfff)
                codepoint |= ch - 0xdc00;
            else
                codepoint = ch;

            if (codepoint <= 0x7f)
                out.append(1, static_cast<char>(codepoint));
            else if (codepoint <= 0x7ff)
            {
                out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
                out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
            }
            else if (codepoint <= 0xffff)
            {
                out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
                out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
                out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
            }
            else
            {
                out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
                out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
                out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
                out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
            }
            codepoint = 0;
        }
    }
    return out;
#endif
}

/**
 * Takes a wide-character string and converts it to an
 * 8-bit string encoded in the current system codepage.
 * @param src Wide-character string.
 * @return Codepage string.
 */
std::string Language::wstrToCp(const std::wstring& src)
{
	if (src.empty())
		return "";
#ifdef _WIN32
	int size = WideCharToMultiByte(CP_ACP, 0, &src[0], (int)src.size(), NULL, 0, NULL, NULL);
	std::string str(size, 0);
	WideCharToMultiByte(CP_ACP, 0, &src[0], (int)src.size(), &str[0], size, NULL, NULL);
	return str;
#else
	const int MAX = 500;
	char buffer[MAX];
	setlocale(LC_ALL, "");
	wcstombs(buffer, src.c_str(), MAX);
	setlocale(LC_ALL, "C");
	std::string str(buffer);
	return str;
#endif
}

/**
 * Takes a wide-character string and converts it to an
 * 8-bit string with the filesystem encoding.
 * @param src Wide-character string.
 * @return Filesystem string.
 */
std::string Language::wstrToFs(const std::wstring& src)
{
#ifdef _WIN32
	return Language::wstrToCp(src);
#else
	return Language::wstrToUtf8(src);
#endif
}

/**
 * Takes an 8-bit string encoded in UTF-8 and converts it
 * to a wide-character string.
 * @note Adapted from http://stackoverflow.com/questions/148403/utf8-to-from-wide-char-conversion-in-stl
 * @param src UTF-8 string.
 * @return Wide-character string.
 */
std::wstring Language::utf8ToWstr(const std::string& src)
{
	if (src.empty())
		return L"";
#ifdef _WIN32
	int size = MultiByteToWideChar(CP_UTF8, 0, &src[0], (int)src.size(), NULL, 0);
    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, &src[0], (int)src.size(), &wstr[0], size);
	return wstr;
#else
	std::wstring out;
    unsigned int codepoint = 0;
    int following = 0;
    for (std::string::const_iterator i = src.begin(); i != src.end(); ++i)
    {
        unsigned char ch = *i;
        if (ch <= 0x7f)
        {
            codepoint = ch;
            following = 0;
        }
        else if (ch <= 0xbf)
        {
            if (following > 0)
            {
                codepoint = (codepoint << 6) | (ch & 0x3f);
                --following;
            }
        }
        else if (ch <= 0xdf)
        {
            codepoint = ch & 0x1f;
            following = 1;
        }
        else if (ch <= 0xef)
        {
            codepoint = ch & 0x0f;
            following = 2;
        }
        else
        {
            codepoint = ch & 0x07;
            following = 3;
        }
        if (following == 0)
        {
            if (codepoint > 0xffff)
            {
                out.append(1, static_cast<wchar_t>(0xd800 + (codepoint >> 10)));
                out.append(1, static_cast<wchar_t>(0xdc00 + (codepoint & 0x03ff)));
            }
            else
                out.append(1, static_cast<wchar_t>(codepoint));
            codepoint = 0;
        }
    }
    return out;
#endif
}

/**
 * Takes an 8-bit string encoded in the current system codepage
 * and converts it to a wide-character string.
 * @param src Codepage string.
 * @return Wide-character string.
 */
std::wstring Language::cpToWstr(const std::string& src)
{
	if (src.empty())
		return L"";
#ifdef _WIN32
	int size = MultiByteToWideChar(CP_ACP, 0, &src[0], (int)src.size(), NULL, 0);
    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_ACP, 0, &src[0], (int)src.size(), &wstr[0], size);
	return wstr;
#else
	const int MAX = 500;
	wchar_t buffer[MAX + 1];
	setlocale(LC_ALL, "");
	size_t len = mbstowcs(buffer, src.c_str(), MAX);
	setlocale(LC_ALL, "C");
	if (len == (size_t)-1)
		return L"?";
	return std::wstring(buffer, len);
#endif
}

/**
 * Takes an 8-bit string with the filesystem encoding
 * and converts it to a wide-character string.
 * @param src Filesystem string.
 * @return Wide-character string.
 */
std::wstring Language::fsToWstr(const std::string& src)
{
#ifdef _WIN32
	return Language::cpToWstr(src);
#else
	return Language::utf8ToWstr(src);
#endif
}

/**
 * Replaces every instance of a substring.
 * @param str The string to modify.
 * @param find The substring to find.
 * @param replace The substring to replace it with.
 */
void Language::replace(std::string &str, const std::string &find, const std::string &replace)
{
	for (size_t i = str.find(find); i != std::string::npos; i = str.find(find, i + replace.length()))
	{
		str.replace(i, find.length(), replace);
	}
}

/**
 * Replaces every instance of a substring.
 * @param str The string to modify.
 * @param find The substring to find.
 * @param replace The substring to replace it with.
 */
void Language::replace(std::wstring &str, const std::wstring &find, const std::wstring &replace)
{
	for (size_t i = str.find(find); i != std::wstring::npos; i = str.find(find, i + replace.length()))
	{
		str.replace(i, find.length(), replace);
	}
}

/**
 * Gets all the languages found in the
 * data folder and adds them to a text list.
 * @param list Text list.
 * @return List of language filenames.
 */
std::vector<std::string> Language::getList(TextList *list)
{
	std::vector<std::string> langs = CrossPlatform::getFolderContents(CrossPlatform::getDataFolder("Language/"), "yml");

	for (std::vector<std::string>::iterator i = langs.begin(); i != langs.end(); ++i)
	{
		(*i) = CrossPlatform::noExt(*i);
		if (list != 0)
		{
			std::wstring name;
			std::map<std::string, std::wstring>::iterator lang = _names.find(*i);
			if (lang != _names.end())
			{
				name = lang->second;
			}
			else
			{
				name = Language::fsToWstr(*i);
			}
			list->addRow(1, name.c_str());
		}
	}
	return langs;
}

/**
 * Loads a language file in Ruby-on-Rails YAML format.
 * Not that his has anything to do with Ruby, but since it's
 * a widely-supported format and we already have YAML
 * it was convenient.
 * @param filename Filename of the YAML file.
 * @param extras Pointer to extra strings from ruleset.
 */
void Language::load(const std::string &filename, ExtraStrings *extras)
{
	_strings.clear();

	YAML::Node doc = YAML::LoadFile(filename);
	_id = doc.begin()->first.as<std::string>();
	YAML::Node lang = doc.begin()->second;
	for (YAML::const_iterator i = lang.begin(); i != lang.end(); ++i)
	{
		// Regular strings
		if (i->second.IsScalar())
		{
			_strings[i->first.as<std::string>()] = loadString(i->second.as<std::string>());
		}
		// Strings with plurality
		else if (i->second.IsMap())
		{
			for (YAML::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
			{
				std::string s = i->first.as<std::string>() + "_" + j->first.as<std::string>();
				_strings[s] = loadString(j->second.as<std::string>());
			}
		}
	}
	delete _handler;
	_handler = LanguagePlurality::create(_id);
	if (extras)
	{
		for (std::map<std::string, std::string>::const_iterator i = extras->getStrings()->begin(); i != extras->getStrings()->end(); ++i)
		{
			_strings[i->first] = loadString(i->second);
		}
	}
}

/**
* Replaces all special string markers with the approriate characters
* and converts the string encoding.
* @param string Original UTF-8 string.
* @return New widechar string.
*/
std::wstring Language::loadString(const std::string &string) const
{
	std::string s = string;
	replace(s, "{NEWLINE}", "\n");
	replace(s, "{SMALLLINE}", "\x02");
	replace(s, "{ALT}", "\x01");
	return utf8ToWstr(s);
}

/**
 * Loads pairs of strings separated by linebreaks contained
 * in a text file into the Language. Each pair is made of
 * an ID and a localized string.
 * @param filename Filename of the LNG file.
 * @param extras Pointer to extra strings from ruleset.
 * @sa @ref LanguageFiles
 */
void Language::loadLng(const std::string &filename, ExtraStrings *extras)
{
	_strings.clear();

	std::ifstream txtFile (filename.c_str(), std::ios::in | std::ios::binary);
	if (!txtFile)
	{
		throw Exception(filename + " not found");
	}
	txtFile.exceptions(std::ios::badbit);

	try
	{
		std::string id, u8msg;
		std::string language;
		// Get language name
		std::getline(txtFile, language);
		_id = language;
		//std::string yaml = "../../../" + language + ".yml";
		//std::ofstream yamlFile(yaml, std::ios::out);
		// Read lines in pairs.
		while (!std::getline(txtFile, id).eof())
		{
			if (std::getline(txtFile, u8msg).fail())
			{
				throw Exception("Invalid language file");
			}
			//replace(u8msg, "\"", "\\\"");
			//yamlFile << id << ": " << '"' << u8msg << '"' << std::endl;
			//replace(u8msg, "\\\"", "\"");
			replace(u8msg, "{NEWLINE}", "\n");
			replace(u8msg, "{SMALLLINE}", "\x02");
			replace(u8msg, "{ALT}", "\x01");
			_strings[id] = utf8ToWstr(u8msg);
		}
		delete _handler;
		_handler = LanguagePlurality::create(language);
		//yamlFile.close();
	}
	catch (std::ifstream::failure e)
	{
		throw Exception("Invalid language file");
	}
	if (extras)
	{
		for (std::map<std::string, std::string>::const_iterator i = extras->getStrings()->begin(); i != extras->getStrings()->end(); ++i)
		{
			std::string s = i->second;
			replace(s, "{NEWLINE}", "\n");
			replace(s, "{SMALLLINE}", "\x02");
			replace(s, "{ALT}", "\x01");
			_strings[i->first] = utf8ToWstr(s);
		}
	}
	txtFile.close();
}

/**
* Returns the language's locale.
* @return IANA language tag.
*/
std::string Language::getId() const
{
	return _id;
}

/**
 * Returns the language's name in its native language.
 * @return Language name.
 */
std::wstring Language::getName() const
{
	return _names[_id];
}

/**
 * Returns the localized text with the specified ID.
 * If it's not found, just returns the ID.
 * @param id ID of the string.
 * @return String with the requested ID.
 */
const LocalizedText &Language::getString(const std::string &id) const
{
	static LocalizedText hack(L"");
	if (id.empty())
		return hack;
	std::map<std::string, LocalizedText>::const_iterator s = _strings.find(id);
	if (s == _strings.end())
	{
		Log(LOG_WARNING) << id << " not found in " << Options::getString("language");
		hack = LocalizedText(utf8ToWstr(id));
		return hack;
	}
	else
	{
		return s->second;
	}
}

/**
 * Returns the localized text with the specified ID, in the proper form for @a n.
 * The substitution of @a n has already happened in the returned LocalizedText.
 * If it's not found, just returns the ID.
 * @param id ID of the string.
 * @param n Number to use to decide the proper form.
 * @return String with the requested ID.
 */
LocalizedText Language::getString(const std::string &id, unsigned n) const
{
	assert(!id.empty());
	std::map<std::string, LocalizedText>::const_iterator s = _strings.end();
	if (0 == n)
	{
		// Try specialized form.
		s = _strings.find(id + "_zero");
	}
	if (s == _strings.end())
	{
		// Try proper form by language
		s = _strings.find(id + _handler->getSuffix(n));
	}
	if (s == _strings.end())
	{
		Log(LOG_WARNING) << id << " not found in " << Options::getString("language");
		return LocalizedText(utf8ToWstr(id));
	}
	std::wstringstream ss;
	ss << n;
	std::wstring marker(L"{N}"), val(ss.str()), txt(s->second);
	replace(txt, marker, val);
	return txt;
}

/**
 * Returns the localized text with the specified ID, in the proper form for the gender.
 * If it's not found, just returns the ID.
 * @param id ID of the string.
 * @return String with the requested ID.
 */
const LocalizedText &Language::getString(const std::string &id, SoldierGender gender) const
{
	std::string genderId;
	if (gender == GENDER_MALE)
	{
		genderId = id + "_MALE";
	}
	else
	{
		genderId = id + "_FEMALE";
	}
	return getString(genderId);
}

/**
 * Outputs all the language IDs and strings
 * to an HTML table.
 * @param filename HTML file.
 */
void Language::toHtml(const std::string &filename) const
{
	std::ofstream htmlFile (filename.c_str(), std::ios::out);
	htmlFile << "<table border=\"1\" width=\"100%\">" << std::endl;
	htmlFile << "<tr><th>ID String</th><th>English String</th></tr>" << std::endl;
	for (std::map<std::string, LocalizedText>::const_iterator i = _strings.begin(); i != _strings.end(); ++i)
	{
		htmlFile << "<tr><td>" << i->first << "</td><td>";
		std::string s = wstrToUtf8(i->second);
		for (std::string::const_iterator j = s.begin(); j != s.end(); ++j)
		{
			if (*j == 2 || *j == '\n')
			{
				htmlFile << "<br />";
			}
			else
			{
				htmlFile << *j;
			}
		}
		htmlFile << "</td></tr>" << std::endl;
	}
	htmlFile << "</table>" << std::endl;
	htmlFile.close();
}

}

/** @page LanguageFiles Format of the language files.

Language files (.lng) contain UTF-8 text.
The first line in a language file is the language's name, in the language itself.
The rest of the file is processed in line pairs. The first line of each pair
contains the ID string (dictionary key), the next line contains the localized
text for the given key.

The localized text may contain the following special markers:
<table>
<tr>
 <td><tt>{</tt><i>1, 2, 3, ...</i> <tt>}</tt></td>
 <td>These markers will be replaced by programmer-supplied values before the
 message is displayed.</td></tr>
<tr>
 <td><tt>{ALT}</tt></td>
 <td>The rest of the text will be in an alternate color. Using this again will
 switch back to the primary color.</td></tr>
<tr>
 <td><tt>{NEWLINE}</tt></td>
 <td>It will be replaced with a line break in the game.</td></tr>
<tr>
 <td>{SMALLLINE}</td>
 <td>The rest of the text will be in a small font.</td></tr>
</table>

There is an additional marker sequence, that should only appear in texts that
depend on a number. This marker <tt>{N}</tt> will be replaced by the actual
number used. The keys for texts that depend on numbers also have special
suffixes, that depend on the language. For all languages, a suffix of
<tt>_0</tt> is tried if the number is zero, before trying the actual key
according to the language rules. The rest of the suffixes depend on the language.

<table>
<caption>Current rules</caption>
<tr><th>Language</th><th>Suffixes</th></tr>
<tr><td>English</td><td><tt>_1</tt> n == 1, <tt>_2</tt> otherwise</td></tr>
<tr><td>French</td><td><tt>_1</tt> n < 2, <tt>_2</tt> otherwise</td></tr>
<tr><td>Czech</td><td><tt>_1</tt> n % 100 == 1, <tt>_2</tt> 2 <= n % 100 <= 4, <tt>_3</tt> otherwise</td></tr>
<tr><td>Polish</td><td><tt>_1</tt> n % 100 == 1,
 <tt>_2</tt> 2 <= n % 10 <= 4 && (n % 100 < 10 || n % 100 > 20),
 <tt>_3</tt> otherwise</td></tr>
<tr><td>Romanian</td><td><tt>_1</tt> n % 100 == 1,
 <tt>_2</tt> n == 0 || 1 <= n % 100 <= 20,
 <tt>_3</tt> otherwise</td></tr>
<tr><td>Russian</td><td><tt>_1</tt> n % 10 == 1 && n % 100 != 11,
 <tt>_2</tt> 2 <= n % 10 <= 4 && (n % 100 < 10 || n % 100 > 20),
 <tt>_3</tt> otherwise</td></tr>
<tr><td>Hungarian</td><td><tt>_1</tt> for every case.</td></tr>
<tr><td><i>Other languages</i></th><td><tt>_1</tt> n == 1, <tt>_2</tt> otherwise</td></tr>
</table>

So, you would write (for English):
<pre>
STR_ENEMIES_0
There are no enemies left.
STR_ENEMIES_1
There is a single enemy left.
STR_ENEMIES_2
There are {N} enemies left.
</pre>

*/
