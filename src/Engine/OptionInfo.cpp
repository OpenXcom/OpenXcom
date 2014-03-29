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
#include "OptionInfo.h"
#include "Exception.h"

namespace OpenXcom
{

OptionInfo::OptionInfo(const std::string &id, bool *option, bool def, const std::string &desc, const std::string &cat) : _id(id), _desc(desc), _cat(cat), _type(OPTION_BOOL)
{
	_ref.b = option;
	_def.b = def;
}

OptionInfo::OptionInfo(const std::string &id, int *option, int def, const std::string &desc, const std::string &cat) : _id(id), _desc(desc), _cat(cat), _type(OPTION_INT)
{
	_ref.i = option;
	_def.i = def;
}

OptionInfo::OptionInfo(const std::string &id, SDLKey *option, SDLKey def, const std::string &desc, const std::string &cat) : _id(id), _desc(desc), _cat(cat), _type(OPTION_KEY)
{
	_ref.k = option;
	_def.k = def;
}

OptionInfo::OptionInfo(const std::string &id, std::string *option, const char *def, const std::string &desc, const std::string &cat) : _id(id), _desc(desc), _cat(cat), _type(OPTION_STRING)
{
	_ref.s = option;
	_def.s = def;
}

void OptionInfo::load(const YAML::Node &node) const
{
	switch (_type)
	{
	case OPTION_BOOL:
		*(_ref.b) = node[_id].as<bool>();
		break;
	case OPTION_INT:
		*(_ref.i) = node[_id].as<int>();
		break;
	case OPTION_KEY:
		*(_ref.k) = (SDLKey)node[_id].as<int>();
		break;
	case OPTION_STRING:
		*(_ref.s) = node[_id].as<std::string>();
		break;
	}
}

void OptionInfo::load(const std::map<std::string, std::string> &map) const
{
	std::string id = _id;
	std::transform(id.begin(), id.end(), id.begin(), ::tolower);
	std::map<std::string, std::string>::const_iterator it = map.find(id);
	if (it != map.end())
	{
		std::string value = it->second;
		std::stringstream ss;
		bool b;
		int i;
		switch (_type)
		{
		case OPTION_BOOL:
			ss << std::boolalpha << value;
			ss >> std::boolalpha >> b;
			*(_ref.b) = b;
			break;
		case OPTION_INT:
			ss << std::dec << value;
			ss >> std::dec >> i;
			*(_ref.i) = i;
			break;
		case OPTION_KEY:
			ss << std::dec << value;
			ss >> std::dec >> i;
			*(_ref.k) = (SDLKey)i;
			break;
		case OPTION_STRING:
			*(_ref.s) = value;
			break;
		}
	}
}

void OptionInfo::save(YAML::Node &node) const
{
	switch (_type)
	{
	case OPTION_BOOL:
		node[_id] = *(_ref.b);
		break;
	case OPTION_INT:
		node[_id] = *(_ref.i);
		break;
	case OPTION_KEY:
		node[_id] = (int)*(_ref.k);
		break;
	case OPTION_STRING:
		node[_id] = *(_ref.s);
		break;
	}
}

void OptionInfo::reset() const
{
	switch (_type)
	{
	case OPTION_BOOL:
		*(_ref.b) = _def.b;
		break;
	case OPTION_INT:
		*(_ref.i) = _def.i;
		break;
	case OPTION_KEY:
		*(_ref.k) = _def.k;
		break;
	case OPTION_STRING:
		*(_ref.s) = _def.s;
		break;
	}
}

OptionType OptionInfo::type() const
{
	return _type;
}

std::string OptionInfo::description() const
{
	return _desc;
}

std::string OptionInfo::category() const
{
	return _cat;
}

bool *OptionInfo::getBool() const
{
	if (_type != OPTION_BOOL)
	{
		throw Exception(_id + " is not a boolean!");
	}
	return _ref.b;
}

int *OptionInfo::getInt() const
{
	if (_type != OPTION_INT)
	{
		throw Exception(_id + " is not an integer!");
	}
	return _ref.i;
}

SDLKey *OptionInfo::getKey() const
{
	if (_type != OPTION_KEY)
	{
		throw Exception(_id + " is not a key!");
	}
	return _ref.k;
}

std::string *OptionInfo::getString() const
{
	if (_type != OPTION_STRING)
	{
		throw Exception(_id + " is not a string!");
	}
	return _ref.s;
}

}
