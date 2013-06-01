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

#include "ExtraStrings.h"

namespace OpenXcom
{

ExtraStrings::ExtraStrings()
{
}


ExtraStrings::~ExtraStrings()
{
}

void ExtraStrings::load(const YAML::Node &node)
{
	for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
	{
		std::string key;
		i.first() >> key;
		if (key == "strings")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string index;
				j.first() >> index;
				std::string translation;
				j.second() >> translation;
				_strings[index] = translation;
			}
		}
	}
}

std::map<std::string, std::string> *ExtraStrings::getStrings()
{
	return &_strings;
}
}