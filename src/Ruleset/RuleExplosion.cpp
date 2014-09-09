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
#include "RuleExplosion.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of Explosion
 * @param type String defining the type.
 */
RuleExplosion::RuleExplosion(const std::string &type) :
	_type(type), _spriteSheet(""), _start(-1), _end(-1), _animSpeed(-1), _density(-1)
{
}

/**
 *
 */
RuleExplosion::~RuleExplosion()
{
}

/**
 * Loads the UFO from a YAML file.
 * @param node YAML node.
 * @param ruleset Ruleset for the UFO.
 */
void RuleExplosion::load(const YAML::Node &node, Ruleset *ruleset, int modIndex)
{
	_type = node["type"].as<std::string>(_type);
	_spriteSheet = node["spriteSheet"].as<std::string>(_spriteSheet);
	if (node["start"])
	{
		_start = node["start"].as<int>(_start);

		// X1.PCK: 8 entries.  Anything else is assumed to be new
		if (_spriteSheet == "X1.PCK" && _start > 8)
			_start += modIndex;
	}

	if (node["end"])
	{
		_end = node["end"].as<int>(_end);

		// X1.PCK: 8 entries.  Anything else is assumed to be new
		if (_spriteSheet == "X1.PCK" && _end > 8)
			_end += modIndex;
	}

	_animSpeed = node["animSpeed"].as<int>(_animSpeed);
	_density = node["density"].as<int>(_density);
}

}