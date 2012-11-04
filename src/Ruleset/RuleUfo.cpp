/*
 * Copyright 2010-2012 OpenXcom Developers.
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
RuleUfo::RuleUfo(const std::string &type) : _type(type), _size("STR_VERY_SMALL"), _sprite(-1), _damageMax(0), _speedMax(0), _accel(0), _power(0), _range(0), _score(0), _reload(0), _breakOffTime(0), _battlescapeTerrainData(0)
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
	for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
	{
		std::string key;
		i.first() >> key;
		if (key == "type")
		{
			i.second() >> _type;
		}
		else if (key == "size")
		{
			i.second() >> _size;
		}
		else if (key == "sprite")
		{
			i.second() >> _sprite;
		}
		else if (key == "damageMax")
		{
			i.second() >> _damageMax;
		}
		else if (key == "speedMax")
		{
			i.second() >> _speedMax;
		}
		else if (key == "accel")
		{
			i.second() >> _accel;
		}
		else if (key == "power")
		{
			i.second() >> _power;
		}
		else if (key == "range")
		{
			i.second() >> _range;
		}
		else if (key == "score")
		{
			i.second() >> _score;
		}
		else if (key == "reload")
		{
			i.second() >> _reload;
		}
		else if (key == "breakOffTime")
		{
			i.second() >> _breakOffTime;
		}
		else if (key == "battlescapeTerrainData")
		{
			std::string name;
			i.second()["name"] >> name;
			RuleTerrain *rule = new RuleTerrain(name);
			rule->load(i.second(), ruleset);
			_battlescapeTerrainData = rule;
		}
	}
}

/**
 * Saves the UFO to a YAML file.
 * @param out YAML emitter.
 */
void RuleUfo::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "type" << YAML::Value << _type;
	out << YAML::Key << "size" << YAML::Value << _size;
	out << YAML::Key << "sprite" << YAML::Value << _sprite;
	out << YAML::Key << "damageMax" << YAML::Value << _damageMax;
	out << YAML::Key << "speedMax" << YAML::Value << _speedMax;
	out << YAML::Key << "accel" << YAML::Value << _accel;
	out << YAML::Key << "power" << YAML::Value << _power;
	out << YAML::Key << "range" << YAML::Value << _range;
	out << YAML::Key << "score" << YAML::Value << _score;
	out << YAML::Key << "reload" << YAML::Value << _reload;
	out << YAML::Key << "breakOffTime" << YAML::Value << _breakOffTime;
	if (_battlescapeTerrainData != 0)
	{
		out << YAML::Key << "battlescapeTerrainData" << YAML::Value;
		_battlescapeTerrainData->save(out);
	}
	out << YAML::EndMap;
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
	else if (_size == "STR_MEDIUM")
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

}