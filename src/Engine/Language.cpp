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
#include <cassert>
#include "CrossPlatform.h"
#include "Logger.h"
#include "Exception.h"
#include "Options.h"
#include "LocalizedText.h"
#include "../Interface/TextList.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace OpenXcom
{

/**
 * This class is the interface used to find plural forms for the different languages.
 * Derived classes implement getKeys() according to the specific language's rules.
 */
class Language::PluralityRules
{
public:
	/// Allow proper destruction through base pointer.
	virtual ~PluralityRules() { /* Empty by design. */ }
	/// Get dictionary key suffix for value of @a n.
	/**
	  @param count The number controlling the plurality.
	  @return Pointer to the zero-terminated suffix string.
	 */
	virtual const char *getSuffix(unsigned n) const = 0;
	/// Create a concrete instance for a given language.
	static PluralityRules *create(const std::string &language);

protected:
	PluralityRules() { /* Empty by design. */ }
private:
	typedef PluralityRules *(*PFCreate)();
	static std::map<std::string, PFCreate> s_factoryFunctions;
};
}

namespace {
using OpenXcom::Language;
/**
 * Plurality rules for English (also used as default).
 * Provide rules for languages that have singular and plural, with zero using the plural form.
 * @langsuffixes _1 for singular and _2 for plural.
 */
class ManyOneMany: public Language::PluralityRules
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static PluralityRules *create() { return new ManyOneMany; }
};

const char *ManyOneMany::getSuffix(unsigned n) const
{
	if (n == 1)
		return "_1";
	return "_2";
}

/**
 * Plurality rules for French.
 * Provide rules for languages that have singular and plural, with zero using the singular form.
 * @langsuffixes _1 for singular and _2 for plural.
 */
class FrenchRules: public Language::PluralityRules
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static PluralityRules *create() { return new FrenchRules; }
};

const char *FrenchRules::getSuffix(unsigned n) const
{
	if (n < 2)
		return "_1";
	return "_2";
}

/**
 * Plurality rules for Czech.
 * @langsuffixes _1 n%100 == 1, 2<= n%100 <= 4, _3 otherwise.
 */
class CzechRules: public Language::PluralityRules
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static PluralityRules *create() { return new CzechRules; }
};

const char *CzechRules::getSuffix(unsigned n) const
{
	if (n%100 == 1)
		return "_1";
	if (n%100 >= 2 && n%100 <=4)
		return "_2";
	return "_3";
}

/**
 * Plurality rules for Polish.
 * @langsuffixes _1 n%100 == 1, 2<= n%100 <= 4 && (n%100<10 || n%100>20), _3 otherwise.
 */
class PolishRules: public Language::PluralityRules
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static PluralityRules *create() { return new PolishRules; }
};

const char *PolishRules::getSuffix(unsigned n) const
{
	if (n%100 == 1)
		return "_1";
	if ( 2 <= n%10 && n%10 <=4 && (n%100 < 10 || n%100 > 20))
		return "_2";
	return "_3";
}

/**
 * Plurality rules for Romanian.
 * @langsuffixes _1 n%100 == 1, _2 n == 0 || 1 <= n%100 <= 20, _3 otherwise.
 */
class RomanianRules: public Language::PluralityRules
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static PluralityRules *create() { return new RomanianRules; }
};

const char *RomanianRules::getSuffix(unsigned n) const
{
	if (n%100 == 1)
		return "_1";
	if (0 == n || (1 <= n%100 && n%100 <= 20))
		return "_2";
	return "_3";
}

/**
 * Plurality rules for Russian.
 * @langsuffixes _1 n%10 == 1 && n%100 != 11, 2<= n%100 <= 4 && (n%100<10 || n%100>20), _3 otherwise.
 */
class RusianRules: public Language::PluralityRules
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static PluralityRules *create() { return new RusianRules; }
};

const char *RusianRules::getSuffix(unsigned n) const
{
	if (n%10 == 1 && n%100 != 11)
		return "_1";
	if ( 2 <= n%10 && n%10 <=4 && (n%100 < 10 || n%100 > 20))
		return "_2";
	return "_3";
}

/**
 * Plurality rules for Hungarian.
 * @langsuffixes _1 for everything.
 */
class HungarianRules: public Language::PluralityRules
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static PluralityRules *create() { return new HungarianRules; }
};

const char *HungarianRules::getSuffix(unsigned n) const
{
	return "_1";
}

}

namespace OpenXcom {

/** A mapping of language to plurality rules.
 * It is populated the first time plurality rules are requested.
 * @see Language::PluralityRules::create
 */
std::map<std::string, Language::PluralityRules::PFCreate> Language::PluralityRules::s_factoryFunctions;

/**
 * Search and create a handler for the plurality rules of @a language.
 * If the language was not found, a default with the same rules as English is returned.
 * @param language The target language.
 * @return A newly created PluralityRules instance for the given language.
 * @todo Make sure the rest of the languages we support in OpenXcom are ok with
 * the English rules for plurality.
 * @internal The first time this is called, we populate the language => rules mapping.
 */
Language::PluralityRules *Language::PluralityRules::create(const std::string &language)
{
	// Populate factory the first time we are called.
	if (s_factoryFunctions.empty())
	{
		s_factoryFunctions.insert(std::make_pair("FRANÇAIS", &FrenchRules::create));
		s_factoryFunctions.insert(std::make_pair("ČESKY", &CzechRules::create));
		s_factoryFunctions.insert(std::make_pair("POLSKI", &PolishRules::create));
		s_factoryFunctions.insert(std::make_pair("ROMÂNĂ", &RomanianRules::create));
		s_factoryFunctions.insert(std::make_pair("РУССКИЙ", &RusianRules::create));
		s_factoryFunctions.insert(std::make_pair("MAGYAR", &HungarianRules::create));
	}
	PFCreate creator = &ManyOneMany::create;
	std::map<std::string, PFCreate>::const_iterator found = s_factoryFunctions.find(language);
	if (found != s_factoryFunctions.end())
	{
		creator = found->second;
	}
	return (*creator)();
}

/**
 * Initializes an empty language file.
 */
Language::Language() : _name(L""), _strings(), _handler(0)
{

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
	wcstombs(buffer, src.c_str(), MAX);
	std::string str(buffer);
	return str;
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
	size_t len = mbstowcs(buffer, src.c_str(), MAX);
	if (len == (size_t)-1)
		return L"?";
	return std::wstring(buffer, len);
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
 * Replaces every instance of a substring.
 * @param str The string to modify.
 * @param find The substring to find.
 * @param replace The substring to replace it with.
 */
void Language::replace(std::wstring &str, const std::wstring &find, const std::wstring &replace)
{
	for (size_t i = str.find(find); i != std::wstring::npos; i = str.find(find, i + 1))
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
				throw Exception(file + " not found");
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
 * @sa @ref LanguageFiles
 */
void Language::loadLng(const std::string &filename)
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
		_name = utf8ToWstr(language);
		// Read lines in pairs.
		while (!std::getline(txtFile, id).eof())
		{
			if (std::getline(txtFile, u8msg).fail())
			{
				throw Exception("Invalid language file");
			}
			replace(u8msg, "{NEWLINE}", "\n");
			replace(u8msg, "{SMALLLINE}", "\x02");
			replace(u8msg, "{ALT}", "\x01");
			_strings[id] = utf8ToWstr(u8msg);
		}
		delete _handler;
		_handler = PluralityRules::create(language);
	}
	catch (std::ifstream::failure e)
	{
		throw Exception("Invalid language file");
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
 * Returns the localized text with the specified ID.
 * If it's not found, just returns the ID.
 * @param id ID of the string.
 * @return String with the requested ID.
 */
const LocalizedText &Language::getString(const std::string &id) const
{
	static LocalizedText hack(L"");
	// assert(!id.empty()); // Isn't an empty ID an error?
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
		s = _strings.find(id + "_0");
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
