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
#include "LanguagePlurality.h"

namespace OpenXcom
{

/**
 * Plurality rules for English (also used as default).
 * Provide rules for languages that have singular and plural, with zero using the plural form.
 * @langsuffixes _1 for singular and _2 for plural.
 */
class ManyOneMany: public LanguagePlurality
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static LanguagePlurality *create() { return new ManyOneMany; }
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
class FrenchRules: public LanguagePlurality
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static LanguagePlurality *create() { return new FrenchRules; }
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
class CzechRules: public LanguagePlurality
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static LanguagePlurality *create() { return new CzechRules; }
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
class PolishRules: public LanguagePlurality
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static LanguagePlurality *create() { return new PolishRules; }
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
class RomanianRules: public LanguagePlurality
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static LanguagePlurality *create() { return new RomanianRules; }
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
class RusianRules: public LanguagePlurality
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static LanguagePlurality *create() { return new RusianRules; }
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
class HungarianRules: public LanguagePlurality
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static LanguagePlurality *create() { return new HungarianRules; }
};

const char *HungarianRules::getSuffix(unsigned) const
{
	return "_1";
}

/** A mapping of language to plurality rules.
 * It is populated the first time plurality rules are requested.
 * @see LanguagePlurality::create
 */
std::map<std::string, LanguagePlurality::PFCreate> LanguagePlurality::s_factoryFunctions;

/**
 * Search and create a handler for the plurality rules of @a language.
 * If the language was not found, a default with the same rules as English is returned.
 * @param language The target language.
 * @return A newly created LanguagePlurality instance for the given language.
 * @todo Make sure the rest of the languages we support in OpenXcom are ok with
 * the English rules for plurality.
 * @internal The first time this is called, we populate the language => rules mapping.
 */
LanguagePlurality *LanguagePlurality::create(const std::string &language)
{
	// Populate factory the first time we are called.
	if (s_factoryFunctions.empty())
	{
		s_factoryFunctions.insert(std::make_pair("fr", &FrenchRules::create));
		s_factoryFunctions.insert(std::make_pair("cs-CZ", &CzechRules::create));
		s_factoryFunctions.insert(std::make_pair("pl-PL", &PolishRules::create));
		s_factoryFunctions.insert(std::make_pair("ro", &RomanianRules::create));
		s_factoryFunctions.insert(std::make_pair("ru", &RusianRules::create));
		s_factoryFunctions.insert(std::make_pair("hu-HU", &HungarianRules::create));
	}
	PFCreate creator = &ManyOneMany::create;
	std::map<std::string, PFCreate>::const_iterator found = s_factoryFunctions.find(language);
	if (found != s_factoryFunctions.end())
	{
		creator = found->second;
	}
	return (*creator)();
}

}
