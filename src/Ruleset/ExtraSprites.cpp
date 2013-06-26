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

/*
 * Creates a blank set of extra sprite data.
 */
ExtraSprites::ExtraSprites() : _width(320), _height(200), _singleImage(false), _modIndex(0), _subX(0), _subY(0)
{
}

/*
 * Cleans up the extra sprite set.
 */
ExtraSprites::~ExtraSprites()
{
}

/*
 * Loads the extra sprite set from yaml.
 * @param node YAML node.
 * @param modIndex the internal index of the associated mod.
 */
void ExtraSprites::load(const YAML::Node &node, int modIndex)
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
		else if (key == "subX")
		{
			i.second() >> _subX;
		}
		else if (key == "subY")
		{
			i.second() >> _subY;
		}
		else if (key == "singleImage")
		{
			i.second() >> _singleImage;
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
	_modIndex = modIndex;
}

/*
 * @return the list of sprites defined my this mod.
 */
std::map<int, std::string> *ExtraSprites::getSprites()
{
	return &_sprites;
}

/*
 * @return the width of the surfaces (used for single images and new spritesets)
 */
const int ExtraSprites::getWidth() const
{
	return _width;
}

/*
 * @return the height of the surfaces (used for single images and new spritesets)
 */
const int ExtraSprites::getHeight() const
{
	return _height;
}

/*
 * @return is this a single surface, or a set of surfaces?
 */
const bool ExtraSprites::getSingleImage() const
{
	return _singleImage;
}

/*
 * @return the mod index for this external sprite set.
 */
const int ExtraSprites::getModIndex() const
{
	return _modIndex;
}

const int ExtraSprites::getSubX() const
{
	return _subX;
}

const int ExtraSprites::getSubY() const
{
	return _subY;
}

}