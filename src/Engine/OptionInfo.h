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
#ifndef OPENXCOM_OPTIONINFO_H
#define OPENXCOM_OPTIONINFO_H

#include <yaml-cpp/yaml.h>
#include <string>
#include <map>
#include <SDL.h>

namespace OpenXcom
{

enum OptionType { OPTION_BOOL, OPTION_INT, OPTION_STRING, OPTION_KEY };

class OptionInfo
{
private:
	std::string _id, _desc, _cat;
	OptionType _type;
	union { bool *b; int *i; std::string *s; SDLKey *k; } _ref;
	union { bool b; int i; const char *s; SDLKey k; } _def; // can't put strings in unions
public:
	OptionInfo(const std::string &id, bool *option, bool def, const std::string &desc = "", const std::string &cat = "");
	OptionInfo(const std::string &id, int *option, int def, const std::string &desc = "", const std::string &cat = "");
	OptionInfo(const std::string &id, SDLKey *option, SDLKey def, const std::string &desc = "", const std::string &cat = "");
	OptionInfo(const std::string &id, std::string *option, const char *def, const std::string &desc = "", const std::string &cat = "");
	bool *getBool() const;
	int *getInt() const;
	std::string *getString() const;
	SDLKey *getKey() const;
	void load(const YAML::Node &node) const;
	void load(const std::map<std::string, std::string> &map) const;
	void save(YAML::Node &node) const;
	void reset() const;
	OptionType type() const;
	std::string description() const;
	std::string category() const;
};

}

#endif
