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
#include "LanguagePlurality.h"

namespace OpenXcom
{

/**
 * Default plurality rules.
 * Provide rules for languages where 1 is singular and everything else is plural.
 * @note one = 1; other = ...
 */
class OneSingular: public LanguagePlurality
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static LanguagePlurality *create() { return new OneSingular; }
};

const char *OneSingular::getSuffix(unsigned n) const
{
	if (n == 1)
	{
		return "_one";
	}
	return "_other";
}

/**
 * Plurality rules where 0 is also singular.
 * Provide rules for languages where 0 and 1 are singular and everything else is plural.
 * @note one = 0-1; other = ...
 */
class ZeroOneSingular : public LanguagePlurality
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static LanguagePlurality *create() { return new ZeroOneSingular; }
};

const char *ZeroOneSingular::getSuffix(unsigned n) const
{
	if (n == 0 || n == 1)
	{
		return "_one";
	}
	return "_other";
}

/**
 * Plurality rules where there is no singular.
 * Provide rules for languages where everything is plural.
 * @note other = ...
 */
class NoSingular : public LanguagePlurality
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static LanguagePlurality *create() { return new NoSingular; }
};

const char *NoSingular::getSuffix(unsigned) const
{
	return "_other";
}

/**
 * Plurality rules for Cyrillic languages (Russian, Ukrainian, etc.)
 * @note one = 1, 21, 31...; few = 2-4, 22-24, 32-34...; many = 0, 5-20, 25-30, 35-40...; other = ...
 */
class CyrillicPlurality : public LanguagePlurality
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static LanguagePlurality *create() { return new CyrillicPlurality; }
};

const char *CyrillicPlurality::getSuffix(unsigned n) const
{
	if (n % 10 == 1 && n % 100 != 11)
	{
		return "_one";
	}
	else if ((n % 10 >= 2 && n % 10 <= 4) &&
			!(n % 100 >= 12 && n % 100 <= 14))
	{
		return "_few";
	}
	else if (n % 10 == 0 ||
			(n % 10 >= 5 && n % 10 <= 9) ||
			(n % 100 >= 11 && n % 100 <= 14))
	{
		return "_many";
	}
	return "_other";
}

/**
 * Plurality rules for Czech and Slovak languages.
 * @note one = 1; few = 2-4; other = ...
 */
class CzechPlurality : public LanguagePlurality
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static LanguagePlurality *create() { return new CzechPlurality; }
};

const char *CzechPlurality::getSuffix(unsigned n) const
{
	if (n == 1)
	{
		return "_one";
	}
	else if (n >= 2 && n <= 4)
	{
		return "_few";
	}
	return "_other";
}

/**
 * Plurality rules for the Polish language.
 * @note one = 1; few = 2-4, 22-24, 32-34...; many = 0, 5-21, 25-31, 35-41, ...; other = ...
 */
class PolishPlurality : public LanguagePlurality
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static LanguagePlurality *create() { return new PolishPlurality; }
};

const char *PolishPlurality::getSuffix(unsigned n) const
{
	if (n == 1)
	{
		return "_one";
	}
	else if ((n % 10 >= 2 && n % 10 <= 4) &&
			!(n % 100 >= 12 && n % 100 <= 14))
	{
		return "_few";
	}
	else if ((n % 10 <= 1) ||
			(n % 10 >= 5 && n % 10 <= 9) ||
			(n % 100 >= 12 && n % 100 <= 14))
	{
		return "_many";
	}
	return "_other";
}

/**
 * Plurality rules for Romanian and Moldavian languages.
 * @note one = 1; few = 0, 2-19, 101-119...; other = ...
 */
class RomanianPlurality : public LanguagePlurality
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static LanguagePlurality *create() { return new RomanianPlurality; }
};

const char *RomanianPlurality::getSuffix(unsigned n) const
{
	if (n == 1)
	{
		return "_one";
	}
	else if (n == 0 ||
			(n % 100 >= 1 && n % 100 <= 19))
	{
		return "_few";
	}
	return "_other";
}

/**
 * Plurality rules for Croatian and Serbian languages.
 * @note one = 1, 21, 31...; few = 2-4, 22-24, 32-34, ...; other = ...
 */
class CroatianPlurality : public LanguagePlurality
{
public:
	virtual const char *getSuffix(unsigned n) const;
	static LanguagePlurality *create() { return new CroatianPlurality; }
};

const char *CroatianPlurality::getSuffix(unsigned n) const
{
	if (n % 10 == 1 && n % 100 != 11)
	{
		return "_one";
	}
	else if ((n % 10 >= 2 && n % 10 <= 4) &&
			!(n % 100 >= 12 && n % 100 <= 14))
	{
		return "_few";
	}
	return "_other";
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
 * @internal The first time this is called, we populate the language => rules mapping.
 */
LanguagePlurality *LanguagePlurality::create(const std::string &language)
{
	// Populate factory the first time we are called.
	if (s_factoryFunctions.empty())
	{
		s_factoryFunctions.insert(std::make_pair("fr", &ZeroOneSingular::create));
		s_factoryFunctions.insert(std::make_pair("fr-CA", &ZeroOneSingular::create));
		s_factoryFunctions.insert(std::make_pair("hu", &NoSingular::create));
		s_factoryFunctions.insert(std::make_pair("tr", &NoSingular::create));
		s_factoryFunctions.insert(std::make_pair("cs", &CzechPlurality::create));
		s_factoryFunctions.insert(std::make_pair("pl", &PolishPlurality::create));
		s_factoryFunctions.insert(std::make_pair("ro", &RomanianPlurality::create));
		s_factoryFunctions.insert(std::make_pair("ru", &CyrillicPlurality::create));
		s_factoryFunctions.insert(std::make_pair("sk", &CzechPlurality::create));
		s_factoryFunctions.insert(std::make_pair("uk", &CyrillicPlurality::create));
		s_factoryFunctions.insert(std::make_pair("ja", &NoSingular::create));
		s_factoryFunctions.insert(std::make_pair("ko", &NoSingular::create));
		s_factoryFunctions.insert(std::make_pair("zh-CN", &NoSingular::create));
		s_factoryFunctions.insert(std::make_pair("zh-TW", &NoSingular::create));
		s_factoryFunctions.insert(std::make_pair("hr", &CroatianPlurality::create));
	}
	PFCreate creator = &OneSingular::create;
	std::map<std::string, PFCreate>::const_iterator found = s_factoryFunctions.find(language);
	if (found != s_factoryFunctions.end())
	{
		creator = found->second;
	}
	return (*creator)();
}

}
