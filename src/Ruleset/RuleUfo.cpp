/*
 * Copyright 2010 OpenXcom Developers.
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

/**
 * Creates a blank ruleset for a certain
 * type of UFO.
 * @param type String defining the type.
 */
RuleUfo::RuleUfo(std::string type) : _type(type), _size("STR_VERY_SMALL"), _sprite(-1), _damageMax(0), _speedMax(0), _accel(0), _power(0), _range(0), _score(0), _battlescapeTerrainData(0)
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
 * Changes the size of this type of UFO.
 * @param size Size.
 */
void RuleUfo::setSize(std::string size)
{
	_size = size;
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

/*
 * Changes the ID of the sprite used to draw the UFO
 * in the Dogfight window.
 * @param sprite Sprite ID.
 */
void RuleUfo::setSprite(int sprite)
{
	_sprite = sprite;
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
 * Changes the maximum damage (damage the UFO can take)
 * of the UFO.
 * @param damage Damage.
 */
void RuleUfo::setMaxDamage(int damage)
{
	_damageMax = damage;
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
 * Changes the maximum speed of the UFO flying
 * around the Geoscape.
 * @param speed Speed.
 */
void RuleUfo::setMaxSpeed(int speed)
{
	_speedMax = speed;
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
 * Changes the acceleration of the UFO for
 * taking off / stopping.
 * @param accel Acceleration.
 */
void RuleUfo::setAcceleration(int accel)
{
	_accel = accel;
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
 * Changes the maximum damage done by the
 * UFO's weapons per shot.
 * @param power Weapon power.
 */
void RuleUfo::setWeaponPower(int power)
{
	_power = power;
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
 * Changes the maximum range for the
 * UFO's weapons.
 * @param range Weapon range.
 */
void RuleUfo::setWeaponRange(int range)
{
	_range = range;
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
 * Changes the amount of points the player
 * gets for shooting down the UFO.
 * @param score Score.
 */
void RuleUfo::setScore(int score)
{
	_score = score;
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
 * Changes the terrain data needed to draw the UFO in the battlescape.
 * @param t Terrain.
 */
void RuleUfo::setBattlescapeTerrainData(RuleTerrain *t)
{
	_battlescapeTerrainData = t;
}
