/*
 * Copyright 2010-2014 OpenXcom Developers.
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

#include "RuleMusic.h"
#include <climits>

namespace OpenXcom
{
/**
 * initialize catpos as int_max to prevent trying to load files from cats that don't exist,
 * but allow for optional music to be listed regardless for loading .ogg or .mp3 versions
 * of said files, should they be present.
 * default normalization value is 0.76, this only applies to the adlib mixer as far as i know.
 * also, 0.76 is roughly optimal for all the TFTD tracks.
 */
RuleMusic::RuleMusic(const std::string id) : _catPos(INT_MAX), _normalization(0.76) 
{
}

RuleMusic::~RuleMusic()
{
}

/**
 * Loads info about the music track.
 * @param node yaml node to read from.
 */
void RuleMusic::load(const YAML::Node& node)
{
	_catPos = node["catPos"].as<int>(_catPos);
	_normalization = node["normalization"].as<float>(_normalization);
}

/**
 * Gets the track's index in the catalog file.
 * @return the track's index in the file.
 */
int RuleMusic::getCatPos()
{
	return _catPos;
}

/**
 * Gets the track's normalization level.
 * @return the track's normalization value.
 */
float RuleMusic::getNormalization()
{
	return _normalization;
}

}