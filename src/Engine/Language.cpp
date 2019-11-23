/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include <fstream>
#include <cassert>
#include <set>
#include <climits>
#include <algorithm>
#include "CrossPlatform.h"
#include "Logger.h"
#include "Options.h"
#include "LanguagePlurality.h"
#include "Unicode.h"
#include "Exception.h"
#include "../Mod/ExtraStrings.h"

namespace OpenXcom
{

std::map<std::string, std::string> Language::_names;
std::vector<std::string> Language::_rtl, Language::_cjk;

/**
 * Initializes an empty language file.
 */
Language::Language() : _handler(0), _direction(DIRECTION_LTR), _wrap(WRAP_WORDS)
{
	// maps don't have initializers :(
	if (_names.empty())
	{
		_names["en-US"] = "English (US)";
		_names["en-GB"] = "English (UK)";
		_names["bg"] = "Български";
		_names["cs"] = "Česky";
		_names["cy"] = "Cymraeg";
		_names["da"] = "Dansk";
		_names["de"] = "Deutsch";
		_names["el"] = "Ελληνικά";
		_names["et"] = "Eesti";
		_names["es-ES"] = "Español (ES)";
		_names["es-419"] = "Español (AL)";
		_names["fr"] = "Français (FR)";
		_names["fr-CA"] = "Français (CA)";
		_names["fi"] = "Suomi";
		_names["hr"] = "Hrvatski";
		_names["hu"] = "Magyar";
		_names["it"] = "Italiano";
		_names["ja"] = "日本語";
		_names["ko"] = "한국어";
		_names["lb"] = "Lëtzebuergesch";
		_names["lv"] = "Latviešu";
		_names["nl"] = "Nederlands";
		_names["no"] = "Norsk";
		_names["pl"] = "Polski";
		_names["pt-BR"] = "Português (BR)";
		_names["pt-PT"] = "Português (PT)";
		_names["ro"] = "Română";
		_names["ru"] = "Русский";
		_names["sk"] = "Slovenčina";
		_names["sl"] = "Slovenščina";
		_names["sv"] = "Svenska";
		_names["th"] = "ไทย";
		_names["tr"] = "Türkçe";
		_names["uk"] = "Українська";
		_names["zh-CN"] = "中文";
		_names["zh-TW"] = "文言";
	}
	if (_rtl.empty())
	{
		//_rtl.push_back("he"); needs translation
	}
	if (_cjk.empty())
	{
		_cjk.push_back("ja");
		//_cjk.push_back("ko");  has spacing between words
		_cjk.push_back("zh-CN");
		_cjk.push_back("zh-TW");
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
 * Gets all the languages found in the
 * Data folder and returns their properties.
 * @param files List of language filenames.
 * @param names List of language human-readable names.
 */
void Language::getList(std::vector<std::string> &files, std::vector<std::string> &names)
{
	files = CrossPlatform::getFolderContents(CrossPlatform::searchDataFolder("common/Language"), "yml");
	names.clear();

	for (std::vector<std::string>::iterator i = files.begin(); i != files.end(); ++i)
	{
		*i = CrossPlatform::noExt(*i);
		std::string name;
		std::map<std::string, std::string>::iterator lang = _names.find(*i);
		if (lang != _names.end())
		{
			name = lang->second;
		}
		else
		{
			name = *i;
		}
		names.push_back(name);
	}
}

/**
 * Loads a language file in Ruby-on-Rails YAML format.
 * Not that this has anything to do with Ruby, but since it's a
 * widely-supported format and we already have YAML, it was convenient.
 * @param filename Filename of the YAML file.
 */
void Language::load(const std::string &filename)
{
	YAML::Node doc = YAML::LoadFile(filename);
	YAML::Node lang;
	if (doc.begin()->second.IsMap())
	{
		_id = doc.begin()->first.as<std::string>();
		lang = doc.begin()->second;
	}
	// Fallback when file is missing language specifier
	else
	{
		_id = CrossPlatform::noExt(CrossPlatform::baseFilename(filename));
		lang = doc;
	}
	for (YAML::const_iterator i = lang.begin(); i != lang.end(); ++i)
	{
		// Regular strings
		if (i->second.IsScalar())
		{
			std::string value = i->second.as<std::string>();
			if (!value.empty())
			{
				std::string key = i->first.as<std::string>();
				_strings[key] = loadString(value);
			}
		}
		// Strings with plurality
		else if (i->second.IsMap())
		{
			for (YAML::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
			{
				std::string value = j->second.as<std::string>();
				if (!value.empty())
				{
					std::string key = i->first.as<std::string>() + "_" + j->first.as<std::string>();
					_strings[key] = loadString(value);
				}
			}
		}
	}
	delete _handler;
	_handler = LanguagePlurality::create(_id);
	if (std::find(_rtl.begin(), _rtl.end(), _id) == _rtl.end())
	{
		_direction = DIRECTION_LTR;
	}
	else
	{
		_direction = DIRECTION_RTL;
	}
	if (std::find(_cjk.begin(), _cjk.end(), _id) == _cjk.end())
	{
		_wrap = WRAP_WORDS;
	}
	else
	{
		_wrap = WRAP_LETTERS;
	}
}

/**
 * Loads a language file from an external path.
 * @param path Language file path.
 */
void Language::loadFile(const std::string &path)
{
	try
	{
		if (CrossPlatform::fileExists(path))
		{
			load(path);
		}
	}
	catch (YAML::Exception &e)
	{
		throw Exception(path + ": " + std::string(e.what()));
	}
}

/**
 * Loads a language file from a mod's ExtraStrings.
 * @param extraStrings List of ExtraStrings.
 * @param id Language ID.
 */
void Language::loadRule(const std::map<std::string, ExtraStrings*> &extraStrings, const std::string &id)
{
	std::map<std::string, ExtraStrings*>::const_iterator it = extraStrings.find(id);
	if (it != extraStrings.end())
	{
		ExtraStrings *extras = it->second;
		for (std::map<std::string, std::string>::const_iterator i = extras->getStrings()->begin(); i != extras->getStrings()->end(); ++i)
		{
			_strings[i->first] = loadString(i->second);
		}
	}
}

/**
 * Replaces all special string markers with the appropriate characters.
 * @param string Original string.
 * @return New converted string.
 */
std::string Language::loadString(const std::string &string) const
{
	std::string s = string;
	Unicode::replace(s, "{NEWLINE}", "\n");
	Unicode::replace(s, "{SMALLLINE}", "\x02"); // Unicode::TOK_NL_SMALL
	Unicode::replace(s, "{ALT}", "\x01"); // Unicode::TOK_COLOR_FLIP
	return s;
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
std::string Language::getName() const
{
	return _names[_id];
}

/**
 * Returns the localized text with the specified ID.
 * If it's not found, just returns the ID.
 * @param id ID of the string.
 * @return String with the requested ID.
 */
LocalizedText Language::getString(const std::string &id) const
{
	if (id.empty())
	{
		return id;
	}
	std::map<std::string, LocalizedText>::const_iterator s = _strings.find(id);
	// Check if translation strings recently learned pluralization.
	if (s == _strings.end())
	{
		return getString(id, UINT_MAX);
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
	static std::set<std::string> notFoundIds;
	std::map<std::string, LocalizedText>::const_iterator s = _strings.end();
	// Try specialized form.
	if (n == 0)
	{
		s = _strings.find(id + "_zero");
	}
	// Try proper form by language
	if (s == _strings.end())
	{
		s = _strings.find(id + _handler->getSuffix(n));
	}
	// Try default form
	if (s == _strings.end())
	{
		s = _strings.find(id + "_other");
	}
	// Give up
	if (s == _strings.end())
	{
		if (notFoundIds.end() == notFoundIds.find(id))
		{
			notFoundIds.insert(id);
			Log(LOG_WARNING) << id << " not found in " << Options::language;
		}
		return id;
	}
	if (n == UINT_MAX) // Special case
	{
		if (notFoundIds.end() == notFoundIds.find(id))
		{
			notFoundIds.insert(id);
			Log(LOG_WARNING) << id << " has plural format in ``" << Options::language << "``. Code assumes singular format.";
//		Hint: Change ``getstring(ID).arg(value)`` to ``getString(ID, value)`` in appropriate files.
		}
		return s->second;
	}
	else
	{
		std::ostringstream ss;
		ss << n;
		std::string marker("{N}"), val(ss.str()), txt(s->second);
		Unicode::replace(txt, marker, val);
		return txt;
	}

}

/**
 * Returns the localized text with the specified ID, in the proper form for the gender.
 * If it's not found, just returns the ID.
 * @param id ID of the string.
 * @param gender Current soldier gender.
 * @return String with the requested ID.
 */
LocalizedText Language::getString(const std::string &id, SoldierGender gender) const
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
		std::string s = i->second;
		for (std::string::const_iterator j = s.begin(); j != s.end(); ++j)
		{
			if (*j == Unicode::TOK_NL_SMALL || *j == '\n')
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

/**
 * Returns the direction to use for rendering
 * text in this language.
 * @return Text direction.
 */
TextDirection Language::getTextDirection() const
{
	return _direction;
}

/**
 * Returns the wrapping rules to use for rendering
 * text in this language.
 * @return Text wrapping.
 */
TextWrapping Language::getTextWrapping() const
{
	return _wrap;
}

}

/** @page LanguageFiles Format of the language files.

Language files are formatted as YAML (.yml) containing UTF-8 (no BOM) text.
The first line in a language file is the language's identifier.
The rest of the file are key-value pairs. The key of each pair
contains the ID string (dictionary key), and the value contains the localized
text for the given key in quotes.

The localized text may contain the following special markers:
<table>
<tr>
 <td><tt>{</tt><i>0, 1, 2, ...</i> <tt>}</tt></td>
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
 <td><tt>{SMALLLINE}</tt></td>
 <td>The rest of the text will be in a small font.</td></tr>
</table>

There is an additional marker sequence, that should only appear in texts that
depend on a number. This marker <tt>{N}</tt> will be replaced by the actual
number used. The keys for texts that depend on numbers also have special
suffixes, that depend on the language. For all languages, a suffix of
<tt>_zero</tt> is tried if the number is zero, before trying the actual key
according to the language rules. The rest of the suffixes depend on the language,
as described <a href="http://www.unicode.org/cldr/charts/latest/supplemental/language_plural_rules.html">here</a>.

So, you would write (for English):
<pre>
STR_ENEMIES:
  zero:  "There are no enemies left."
  one:   "There is a single enemy left."
  other: "There are {N} enemies left."
</pre>

 */
