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

#include "ExtraSprites.h"

namespace OpenXcom
{

ExtraSprites::ExtraSprites() : _width(320), _height(200)
{
}


ExtraSprites::~ExtraSprites()
{
}

void ExtraSprites::load(const YAML::Node &node)
{
	for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
	{
		std::string key;
		i.first() >> key;
		if (key == "width")
		{
			i.second() >> _width;
		}
		else if (key == "height")
		{
			i.second() >> _height;
		}
		else if (key == "files")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				int index;
				j.first() >> index;
				std::string filename;
				j.second() >> filename;
				_sprites[index] = filename;
			}
		}
	}
}

std::map<int, std::string> *ExtraSprites::getSprites()
{
	return &_sprites;
}

int ExtraSprites::getWidth()
{
	return _width;
}

int ExtraSprites::getHeight()
{
	return _height;
}

}