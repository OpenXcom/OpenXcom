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
#ifndef OPENXCOM_RULEEXPLOSION_H
#define OPENXCOM_RULEEXPLOSION_H

#include <string>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class RuleTerrain;
class Ruleset;

/**
 * Represents graphics rules for displaying a particular explosion
 * @sa Explosion
 */
class RuleExplosion
{
private:
	std::string _type;
	std::string _spriteSheet;
	int _start;
	int _end;
	int _animSpeed;
	int _density;

public:
	/// Creates a blank explosion ruleset.
	RuleExplosion(const std::string &type);
	/// Cleans up the explosion ruleset.
	~RuleExplosion();
	/// Loads Explosion data from YAML.
	void load(const YAML::Node& node, Ruleset *ruleset, int modIndex);
	/// Gets the explosion's type.
	std::string getType() const {return _type;}
	/// Gets the name of the spriteshet
	std::string getSpriteSheet() const {return _spriteSheet;}
	/// Gets the ID of the first sprite
	const int getStart() const {return _start;}
	/// Gets the ID of the last sprite
	const int getEnd() const {return _end;}
	/// Gets the animation speed (100 is "normal")
	const int getAnimSpeed() const {return _animSpeed;}
	/// Gets the density of explosion sprites to use (100 is "normal")
	const int getDensity() const {return _density;}
};

}

#endif
