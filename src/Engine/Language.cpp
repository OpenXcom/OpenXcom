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
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "Language.h"
#include <locale>
#include <fstream>
#include "CrossPlatform.h"
#include "Logger.h"
#include "Exception.h"
#include "Options.h"
#include "../Interface/TextList.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace OpenXcom
{

/**
 * Initializes an empty language file.
 */
Language::Language() : _name(L""), _strings()
{

}

/**
 *
 */
Language::~Language()
{

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
 * Replaces every instance of a substring.
 * @param str The string to modify.
 * @param find The substring to find.
 * @param replace The substring to replace it with.
 */
void Language::replace(std::string &str, const std::string &find, const std::string &replace)
{
	for (size_t i = str.find(find); i != std::string::npos;)
	{
		str.replace(i, find.length(), replace);
		++i;
		i = str.find(find, i);
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
	std::vector<std::string> langs = CrossPlatform::getFolderContents(Options::getDataFolder() + "Language/", "lng");

	for (std::vector<std::string>::iterator i = langs.begin(); i != langs.end();)
	{
		std::string file = (*i);
		std::string fullname = Options::getDataFolder() + "Language/" + file;
		std::ifstream fin(fullname.c_str(), std::ios::in | std::ios::binary);
		try
		{
			if (!fin)
			{
				throw Exception("Failed to load language");
			}
			char value;
			std::string langname;
			while (fin.read(&value, 1))
			{
				if (value != '\n')
				{
					langname += value;
				}
				else
				{
					break;
				}
			}
			fin.close();
			if (list != 0)
			{
				list->addRow(1, Language::utf8ToWstr(langname).c_str());
			}
			(*i) = file.substr(0, file.length()-4);
			++i;
		}
		catch (Exception &e)
		{
			Log(LOG_ERROR) << e.what();
			i = langs.erase(i);
			continue;
		}
	}
	return langs;
}

/**
 * Loads pairs of strings separated by linebreaks contained
 * in a text file into the Language. Each pair is made of
 * an ID and a localized string.
 * @param filename Filename of the LNG file.
 * @sa <a href="../language_id.html">Reference Table</a>
 */
void Language::loadLng(const std::string &filename)
{
	_strings.clear();

	std::ifstream txtFile (filename.c_str(), std::ios::in | std::ios::binary);
	if (!txtFile)
	{
		throw Exception("Failed to load LNG");
	}

	char value;
	std::string buffer, bufid;
	std::wstring bufstr;
	bool first = true, id = true;

	while (txtFile.read(&value, 1))
	{
		if (value != '\n')
		{
			buffer += value;
		}
		else
		{
			// Get language name
			if (first)
			{
				_name = utf8ToWstr(buffer);
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
					replace(buffer, "{NEWLINE}", "\n");
					replace(buffer, "{SMALLLINE}", "\x02");
					bufstr = utf8ToWstr(buffer);
					_strings[bufid] = bufstr;
				}
				id = !id;
			}
			buffer.clear();
		}
	}

	if (!txtFile.eof())
	{
		throw Exception("Invalid data from file");
	}

	txtFile.close();
}

/**
 * Returns the language's name in its native language.
 * @return Language name.
 */
std::wstring Language::getName() const
{
	return _name;
}

/**
 * Returns the localizable string with the specified ID.
 * If it's not found, just returns the ID.
 * @param id ID of the string.
 * @return String with the requested ID.
 */
std::wstring Language::getString(const std::string &id) const
{
	if (id.empty())
		return L"";
	std::map<std::string, std::wstring>::const_iterator s = _strings.find(id);
	if (s == _strings.end())
	{
		Log(LOG_WARNING) << id << " not found in " << Options::getString("language");
		return utf8ToWstr(id);
	}
	else
	{
		return s->second;
	}
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
	for (std::map<std::string, std::wstring>::const_iterator i = _strings.begin(); i != _strings.end(); ++i)
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
