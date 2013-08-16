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
#include "RuleUfo.h"
#include "RuleTerrain.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of UFO.
 * @param type String defining the type.
 */
RuleUfo::RuleUfo(const std::string &type) : _type(type), _size("STR_VERY_SMALL"), _sprite(-1), _damageMax(0), _speedMax(0), _accel(0), _power(0), _range(0), _score(0), _reload(0), _breakOffTime(0), _battlescapeTerrainData(0), _modSprite("")
{
}

/**
 *
 */
RuleUfo::~RuleUfo()
{
	delete _battlescapeTerrainData;
}

/**
 * Loads the UFO from a YAML file.
 * @param node YAML node.
 * @param ruleset Ruleset for the UFO.
 */
void RuleUfo::load(const YAML::Node &node, Ruleset *ruleset)
{
	_type = node["type"].as<std::string>(_type);
	_size = node["size"].as<std::string>(_size);
	_sprite = node["sprite"].as<int>(_sprite);
	_damageMax = node["damageMax"].as<int>(_damageMax);
	_speedMax = node["speedMax"].as<int>(_speedMax);
	_accel = node["accel"].as<int>(_accel);
	_power = node["power"].as<int>(_power);
	_range = node["range"].as<int>(_range);
	_score = node["score"].as<int>(_score);
	_reload = node["reload"].as<int>(_reload);
	_breakOffTime = node["breakOffTime"].as<int>(_breakOffTime);
	if (const YAML::Node &terrain = node["battlescapeTerrainData"])
	{
		RuleTerrain *rule = new RuleTerrain(terrain["name"].as<std::string>());
		rule->load(terrain, ruleset);
		_battlescapeTerrainData = rule;
	}
	_modSprite = node["modSprite"].as<std::string>(_modSprite);
}


/**
 * Returns the language string that names
 * this UFO. Each UFO type has a unique name.
 * @return Ufo name.
 */
std::string RuleUfo::getType() const
{
	return _type;
}

/**
 * Returns the size of this type of UFO.
 * @return Size.
 */
std::string RuleUfo::getSize() const
{
	return _size;
}

/**
 * Returns the radius of this type of UFO
 * on the dogfighting window.
 * @return Radius in pixels.
 */
int RuleUfo::getRadius() const
{
	if (_size == "STR_VERY_SMALL")
	{
		return 2;
	}
	else if (_size == "STR_SMALL")
	{
		return 3;
	}
	else if (_size == "STR_MEDIUM_UC")
	{
		return 4;
	}
	else if (_size == "STR_LARGE")
	{
		return 5;
	}
	else if (_size == "STR_VERY_LARGE")
	{
		return 6;
	}
	return 0;
}

/*
 * Returns the ID of the sprite used to draw the UFO
 * in the Dogfight window.
 * @return Sprite ID.
 */
int RuleUfo::getSprite() const
{
	return _sprite;
}

/**
 * Returns the maximum damage (damage the UFO can take)
 * of the UFO.
 * @return Damage.
 */
int RuleUfo::getMaxDamage() const
{
	return _damageMax;
}

/**
 * Returns the maximum speed of the UFO flying
 * around the Geoscape.
 * @return Speed.
 */
int RuleUfo::getMaxSpeed() const
{
	return _speedMax;
}

/**
 * Returns the acceleration of the UFO for
 * taking off / stopping.
 * @return Acceleration.
 */
int RuleUfo::getAcceleration() const
{
	return _accel;
}

/**
 * Returns the maximum damage done by the
 * UFO's weapons per shot.
 * @return Weapon power.
 */
int RuleUfo::getWeaponPower() const
{
	return _power;
}

/**
 * Returns the maximum range for the
 * UFO's weapons.
 * @return Weapon range.
 */
int RuleUfo::getWeaponRange() const
{
	return _range;
}

/**
 * Returns the amount of points the player
 * gets for shooting down the UFO.
 * @return Score.
 */
int RuleUfo::getScore() const
{
	return _score;
}

/**
 * Returns the terrain data needed to draw the UFO in the battlescape.
 * @return RuleTerrain.
 */
RuleTerrain *RuleUfo::getBattlescapeTerrainData()
{
	return _battlescapeTerrainData;
}

/**
 * Gets weapon reload for UFO ships.
 * @return UFO weapon relod time.
 */
int RuleUfo::getWeaponReload() const
{
	return _reload;
}

/**
 * Gets UFO break off time.
 * @return UFO break off time in game seconds.
 */
int RuleUfo::getBreakOffTime() const
{
	return _breakOffTime;
}

/**
 * For user-defined UFOs, use a surface for the "preview" image
 * @return the name of the surface that represents this UFO
 */
std::string RuleUfo::getModSprite() const
{
	return _modSprite;
}

}