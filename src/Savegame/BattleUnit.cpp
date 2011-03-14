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
#define _USE_MATH_DEFINES
#include "BattleUnit.h"
#include <cmath>
#include "../Engine/Palette.h"
#include "../Engine/Language.h"
#include "../Battlescape/Pathfinding.h"
#include "Alien.h"

namespace OpenXcom
{

/**
 * Initializes a BattleUnit.
 * @param rules Pointer to RuleUnit object.
 * @param faction Which faction the units belongs to.
 */
BattleUnit::BattleUnit(Unit *unit, UnitFaction faction) : _unit(unit), _faction(faction), _id(0), _pos(Position()), _lastPos(Position()), _direction(0), _status(STATUS_STANDING), _walkPhase(0), _cached(false), _kneeled(false)
{
	_tu = unit->getTimeUnits();
	_energy = unit->getStamina();
	_health = unit->getHealth();
	_morale = 100;
}

/**
 *
 */
BattleUnit::~BattleUnit()
{

}

/**
 * Loads the unit from a YAML file.
 * @param node YAML node.
 */
void BattleUnit::load(const YAML::Node &node)
{
	int a;

	node["id"] >> _id;
	std::string name;
	node["faction"] >> a;
	_faction = (UnitFaction)a;
	node["status"] >> a;
	_status = (UnitStatus)a;

	node["X"] >> _pos.x;
	node["Y"] >> _pos.y;
	node["Z"] >> _pos.z;
	node["direction"] >> _direction;

	node["tu"] >> _tu;
	node["health"] >> _health;
	node["energy"] >> _energy;
	node["morale"] >> _morale;
}

/**
 * Saves the soldier to a YAML file.
 * @param out YAML emitter.
 */
void BattleUnit::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;

	out << YAML::Key << "id" << YAML::Value << _id;
	out << YAML::Key << "faction" << YAML::Value << _faction;
	out << YAML::Key << "status" << YAML::Value << _status;

	out << YAML::Key << "X" << YAML::Value << _pos.x;
	out << YAML::Key << "Y" << YAML::Value << _pos.y;
	out << YAML::Key << "Z" << YAML::Value << _pos.z;
	out << YAML::Key << "direction" << YAML::Value << _direction;

	out << YAML::Key << "tu" << YAML::Value << _tu;
	out << YAML::Key << "health" << YAML::Value << _health;
	out << YAML::Key << "energy" << YAML::Value << _energy;
	out << YAML::Key << "morale" << YAML::Value << _morale;

	out << YAML::EndMap;
}

/**
 * Returns the BattleUnit's unique ID.
 * @return Unique ID.
 */
int BattleUnit::getId() const
{
	return _id;
}

/**
 * Changes the BattleUnit's unique ID.
 * @param id Unique ID.
 */
void BattleUnit::setId(int id)
{
	_id = id;
}

/**
 * Returns the ruleset for the unit's type.
 * @return Pointer to ruleset.
 */
Unit *const BattleUnit::getUnit() const
{
	return _unit;
}

/**
 * Changes the BattleUnit's position.
 * @param pos position
 */
void BattleUnit::setPosition(const Position& pos)
{
	_lastPos = _pos;
	_pos = pos;
}

/**
 * Gets the BattleUnit's position.
 * @return position
 */
const Position& BattleUnit::getPosition() const
{
	return _pos;
}

/**
 * Gets the BattleUnit's position.
 * @return position
 */
const Position& BattleUnit::getLastPosition() const
{
	return _lastPos;
}

/**
 * Gets the BattleUnit's destination.
 * @return destination
 */
const Position& BattleUnit::getDestination() const
{
	return _destination;
}

/**
 * Changes the BattleUnit's direction.
 * @param direction
 */
void BattleUnit::setDirection(int direction)
{
	_direction = direction;
	_toDirection = direction;
}

/**
 * Changes the BattleUnit's direction.
 * @return direction
 */
int BattleUnit::getDirection() const
{
	return _direction;
}

/*
 * Gets the unit's status.
 */
UnitStatus BattleUnit::getStatus()
{
	return _status;
}

void BattleUnit::startWalking(int direction, const Position &destination)
{
	_direction = direction;
	_status = STATUS_WALKING;
	_walkPhase = 0;
	_destination = destination;
	_lastPos = _pos;
	_cached = false;
	_kneeled = false;
}

void BattleUnit::keepWalking()
{
	int middle, end;
	// diagonal walking takes double the steps
	middle = 4 + 4 * (_direction % 2);
	end = 8 + 8 * (_direction % 2);

	_walkPhase++;

	if (_walkPhase == middle)
	{
		// we assume we reached our destination tile
		// this is actually a drawing hack, so soldiers are not overlapped by floortiles
		_pos = _destination;
	}

	if (_walkPhase == end)
	{
		// we officially reached our destination tile
		_status = STATUS_STANDING;
		_walkPhase = 0;
	}

	_cached = false;
}

/*
 * Gets the walking phase for animation and sound.
 * return phase will always go from 0-7
 */
int BattleUnit::getWalkingPhase() const
{
	return _walkPhase % 8;
}

/*
 * Gets the walking phase for diagonal walking.
 * return phase this will be 0 or 8
 */
int BattleUnit::getDiagonalWalkingPhase() const
{
	return (_walkPhase / 8) * 8;
}

void BattleUnit::lookAt(const Position &point)
{
	double ox = point.x - _pos.x;
	double oy = point.y - _pos.y;
	double angle = atan2(ox, oy);
	if (angle < 0) angle += (M_PI*2); // convert to a range from 0 to M_PI*2
	_toDirection = (int)((angle/(M_PI_4))+M_PI_4/2.0); // convert to 8 directions, somewhat rounded
	if (_toDirection > 7) _toDirection = 7;

	if (_toDirection != _direction)
	{
		_status = STATUS_TURNING;
	}
}

void BattleUnit::lookAt(int direction)
{
	_toDirection = direction;
	_status = STATUS_TURNING;
}

void BattleUnit::turn(bool spendTU)
{
    int a = _toDirection - _direction;
    if (a != 0) {
        if (a > 0) {
            if (a <= 4) {
                _direction++;
            } else {
                _direction--;
            }
        } else {
            if (a > -4) {
                _direction--;
            } else {
                _direction++;
            }
        }
        if (_direction < 0) _direction = 7;
        if (_direction > 7) _direction = 0;
		_cached = false;
    }

	// turning always costs 1 tu, except for turning during walking
	if (spendTU)
	{
		_tu--;
	}

	if (_toDirection == _direction)
	{
		// we officially reached our destination
		_status = STATUS_STANDING;
	}
}

/**
 * Returns the unit's faction.
 * @return Faction.
 */
UnitFaction BattleUnit::getFaction() const
{
	return _faction;
}

/**
 * Sets the unit's cache flag.
 * @param cached
 */
void BattleUnit::setCached(bool cached)
{
	_cached = cached;
}

/**
 * Check if the unit is still cached in the Map cache.
 * When the unit changes it needs to be re-cached.
 * @return bool
 */
bool BattleUnit::isCached() const
{
	return _cached;
}

/**
 * Kneel down and spend TUs.
 * @flag to kneel or to stand up
 */
void BattleUnit::kneel(bool kneeled)
{
	_kneeled = kneeled;
	setCached(false);
}

/**
 * Is kneeled down?
 */
bool BattleUnit::isKneeled() const
{
	return _kneeled;
}

/**
 * Aim. (shows the right hand sprite and weapon holding)
 */ 
void BattleUnit::aim(bool aiming)
{
	if (aiming)
		_status = STATUS_AIMING;
	else
		_status = STATUS_STANDING;

	setCached(false);
}

/**
 * Returns the soldier's amount of time units.
 * @return Time units.
 */
int BattleUnit::getTimeUnits() const
{
	return _tu;
}

/**
 * Returns the soldier's amount of stamina.
 * @return Stamina.
 */
int BattleUnit::getEnergy() const
{
	return _energy;
}

/**
 * Returns the soldier's amount of health.
 * @return Health.
 */
int BattleUnit::getHealth() const
{
	return _health;
}

/**
 * Returns the soldier's amount of bravery.
 * @return Bravery.
 */
int BattleUnit::getMorale() const
{
	return _morale;
}

void BattleUnit::damage(Position position, int power)
{
	_health -= power;
	if (_health < 0)
		_health = 0;
}

}
