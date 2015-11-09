/*
 * Copyright 2010-2015 OpenXcom Developers.
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

/**
 * Creates a blank set of extra sprite data.
 */
ExtraSprites::ExtraSprites() : _width(320), _height(200), _singleImage(false), _modIndex(0), _subX(0), _subY(0)
{
}

/**
 * Cleans up the extra sprite set.
 */
ExtraSprites::~ExtraSprites()
{
}

/**
 * Loads the extra sprite set from YAML.
 * @param node YAML node.
 * @param modIndex the internal index of the associated mod.
 */
void ExtraSprites::load(const YAML::Node &node, int modIndex)
{
	_sprites = node["files"].as< std::map<int, std::string> >(_sprites);
	_width = node["width"].as<int>(_width);
	_height = node["height"].as<int>(_height);
	_singleImage = node["singleImage"].as<bool>(_singleImage);
	_subX = node["subX"].as<int>(_subX);
	_subY = node["subY"].as<int>(_subY);
	_modIndex = modIndex;
}

/**
 * Gets the list of sprites defined my this mod.
 * @return The list of sprites.
 */
std::map<int, std::string> *ExtraSprites::getSprites()
{
	return &_sprites;
}

/**
 * Gets the width of the surfaces (used for single images and new spritesets).
 * @return The width of the surfaces.
 */
int ExtraSprites::getWidth() const
{
	return _width;
}

/**
 * Gets the height of the surfaces (used for single images and new spritesets).
 * @return The height of the surfaces.
 */
int ExtraSprites::getHeight() const
{
	return _height;
}

/**
 * Returns whether this is a single surface as opposed to a set of surfaces.
 * @return True if this is a single surface.
 */
bool ExtraSprites::getSingleImage() const
{
	return _singleImage;
}

/**
 * Gets the mod index for this external sprite set.
 * @return The mod index.
 */
int ExtraSprites::getModIndex() const
{
	return _modIndex;
}

/**
 * Gets the x subdivision.
 * @return The x subdivision.
 */
int ExtraSprites::getSubX() const
{
	return _subX;
}

/**
 * Gets the y subdivision.
 * @return The y subdivision.
 */
int ExtraSprites::getSubY() const
{
	return _subY;
}

}
