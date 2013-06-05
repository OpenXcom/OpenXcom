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

#include "ExtraSounds.h"

namespace OpenXcom
{

ExtraSounds::ExtraSounds()
{
}


ExtraSounds::~ExtraSounds()
{
}

void ExtraSounds::load(const YAML::Node &node)
{
	for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
	{
		std::string key;
		i.first() >> key;
		if (key == "files")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				int index;
				j.first() >> index;
				std::string filename;
				j.second() >> filename;
				_sounds[index] = filename;
			}
		}
	}
}

std::map<int, std::string> *ExtraSounds::getSounds()
{
	return &_sounds;
}
}