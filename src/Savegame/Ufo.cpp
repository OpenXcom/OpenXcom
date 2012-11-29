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
#include "Ufo.h"
#include <cmath>
#include <sstream>
#include "../Engine/Language.h"
#include "../Ruleset/RuleUfo.h"
#include "../Savegame/Waypoint.h"

namespace OpenXcom
{

/**
 * Initializes a UFO of the specified type.
 * @param rules Pointer to ruleset.
 */
Ufo::Ufo(RuleUfo *rules)
  : MovingTarget(), _rules(rules), _id(0), _damage(0), _direction("STR_NORTH")
  , _altitude("STR_HIGH_UC"), _detected(false), _hyperDetected(false)
  , _mission("STR_ALIEN_RESEARCH"), _status(FLYING), _timeLeftOnGround(-1)
  , _inBattlescape(false), _shotDownByCraftId(-1)
{
}

/**
 *
 */
Ufo::~Ufo()
{
	delete _dest;
}

/**
 * Loads the UFO from a YAML file.
 * @param node YAML node.
 */
void Ufo::load(const YAML::Node &node)
{
	MovingTarget::load(node);
	node["id"] >> _id;
	node["damage"] >> _damage;
	node["altitude"] >> _altitude;
	node["direction"] >> _direction;
	node["detected"] >> _detected;
	node["hyperDetected"] >> _hyperDetected;
	node["timeOnGround"] >> _timeLeftOnGround;
	node["race"] >> _race;
	node["inBattlescape"] >> _inBattlescape;
	node["mission"] >> _mission;

	double lon, lat;
	node["dest"]["lon"] >> lon;
	node["dest"]["lat"] >> lat;
	_dest = new Waypoint();
	_dest->setLongitude(lon);
	_dest->setLatitude(lat);
	if (_altitude == "STR_GROUND")
	{
		_status = LANDED;
	}
	else
	{
		_status = FLYING;
	}
	if (_damage >= _rules->getMaxDamage())
	{
		_status = DESTROYED;
	}
	else if (_damage >= _rules->getMaxDamage() / 2)
	{
		_status = CRASHED;
	}
}

/**
 * Saves the UFO to a YAML file.
 * @param out YAML emitter.
 */
void Ufo::save(YAML::Emitter &out) const
{
	MovingTarget::save(out);
	out << YAML::Key << "type" << YAML::Value << _rules->getType();
	out << YAML::Key << "id" << YAML::Value << _id;
	out << YAML::Key << "damage" << YAML::Value << _damage;
	out << YAML::Key << "altitude" << YAML::Value << _altitude;
	out << YAML::Key << "direction" << YAML::Value << _direction;
	out << YAML::Key << "detected" << YAML::Value << _detected;
	out << YAML::Key << "hyperDetected" << YAML::Value << _hyperDetected;
	out << YAML::Key << "timeOnGround" << YAML::Value << _timeLeftOnGround;
	out << YAML::Key << "race" << YAML::Value << _race;
	out << YAML::Key << "inBattlescape" << YAML::Value << _inBattlescape;
	out << YAML::Key << "mission" << YAML::Value << _mission;
	out << YAML::EndMap;
}

/**
 * Saves the UFO's unique identifiers to a YAML file.
 * @param out YAML emitter.
 */
void Ufo::saveId(YAML::Emitter &out) const
{
	MovingTarget::saveId(out);
	out << YAML::Key << "type" << YAML::Value << "STR_UFO";
	out << YAML::Key << "id" << YAML::Value << _id;
	out << YAML::EndMap;
}

/**
 * Returns the ruleset for the UFO's type.
 * @return Pointer to ruleset.
 */
RuleUfo *Ufo::getRules() const
{
	return _rules;
}

/**
 * Returns the UFO's unique ID. If it's 0,
 * this UFO has never been detected.
 * @return Unique ID.
 */
int Ufo::getId() const
{
	return _id;
}

/**
 * Changes the UFO's unique ID.
 * @param id Unique ID.
 */
void Ufo::setId(int id)
{
	_id = id;
}

/**
 * Returns the UFO's unique identifying name.
 * @param lang Language to get strings from.
 * @return Full name.
 */
std::wstring Ufo::getName(Language *lang) const
{
	std::wstringstream name;
	switch (_status)
	{
	case FLYING:
		name << lang->getString("STR_UFO_");
		break;
	case LANDED:
		name << lang->getString("STR_LANDING_SITE_");
		break;
	case CRASHED:
		name << lang->getString("STR_CRASH_SITE_");
		break;
	case DESTROYED:
		assert(0 && "Should never happen");
		name << "Destroyed Ufo!";
		break;
	}
	name << _id;
	return name.str();
}

/**
 * Returns the amount of damage this UFO has taken.
 * @return Amount of damage.
 */
int Ufo::getDamage() const
{
	return _damage;
}

/**
 * Changes the amount of damage this UFO has taken.
 * @param damage Amount of damage.
 */
void Ufo::setDamage(int damage)
{
	_damage = damage;
	if (_damage < 0)
	{
		_damage = 0;
	}
	if (_damage >= _rules->getMaxDamage())
	{
		_status = DESTROYED;
	}
	else if (_damage >= _rules->getMaxDamage() / 2)
	{
		_status = CRASHED;
	}
}

/**
 * Returns whether this UFO has been detected by radars.
 * @return Detection status.
 */
bool Ufo::getDetected() const
{
	return _detected;
}

/**
 * Changes whether this UFO has been detected by radars.
 * @param detected Detection status.
 */
void Ufo::setDetected(bool detected)
{
	_detected = detected;
}

/**
 * Returns the amount of remaining hours the UFO has left on the ground.
 * After this many hours thet UFO will take off, if landed, or disappear, if
 * crashed.
 * @return Amount of hours.
 */
int Ufo::getTimeOnGround() const
{
	return _timeLeftOnGround;
}

/**
 * Changes the amount of remaining hours the UFO has left on the ground.
 * After this many hours thet UFO will take off, if landed, or disappear, if
 * crashed.
 * @param hours Amount of hours.
 */
void Ufo::setTimeOnGround(int hours)
{
	_timeLeftOnGround = hours;
}

/**
 * Returns the current direction the UFO is heading in.
 * @return Direction.
 */
std::string Ufo::getDirection() const
{
	return _direction;
}

/**
 * Returns the current altitude of the UFO.
 * @return Altitude.
 */
std::string Ufo::getAltitude() const
{
	return _altitude;
}

/**
 * Changes the current altitude of the UFO.
 * @param altitude Altitude.
 */
void Ufo::setAltitude(const std::string &altitude)
{
	_altitude = altitude;
	if (_altitude != "STR_GROUND")
	{
		_status = FLYING;
	}
	else
	{
		_status = isCrashed() ? CRASHED : LANDED;
	}
}

/**
 * Returns if this UFO took enough damage
 * to cause it to crash.
 * @return Crashed status.
 */
bool Ufo::isCrashed() const
{
	return (_damage >= _rules->getMaxDamage() / 2);
}

/**
 * Returns if this UFO took enough damage
 * to cause it to crash.
 * @return Crashed status.
 */
bool Ufo::isDestroyed() const
{
	return (_damage >= _rules->getMaxDamage());
}

/**
 * Calculates the direction for the UFO based
 * on the current raw speed and destination.
 */
void Ufo::calculateSpeed()
{
	MovingTarget::calculateSpeed();
	if (_speedLon > 0)
	{
		if (_speedLat > 0)
		{
			_direction = "STR_SOUTH_EAST";
		}
		else if (_speedLat < 0)
		{
			_direction = "STR_NORTH_EAST";
		}
		else
		{
			_direction = "STR_EAST";
		}
	}
	else if (_speedLon < 0)
	{
		if (_speedLat > 0)
		{
			_direction = "STR_SOUTH_WEST";
		}
		else if (_speedLat < 0)
		{
			_direction = "STR_NORTH_WEST";
		}
		else
		{
			_direction = "STR_WEST";
		}
	}
	else
	{
		if (_speedLat > 0)
		{
			_direction = "STR_SOUTH";
		}
		else if (_speedLat < 0)
		{
			_direction = "STR_NORTH";
		}
	}
}

/**
 * Moves the UFO to its destination.
 */
void Ufo::think()
{
	switch (_status)
	{
	case FLYING:
		move();
		if (reachedDestination())
		{
			setSpeed(0);
		}
		break;
	case LANDED:
	case CRASHED:
	case DESTROYED:
		// Do nothing
		break;
	}
}

/**
 * Gets the UFO's battlescape status.
 * @return bool
 */
bool Ufo::isInBattlescape() const
{
	return _inBattlescape;
}

/**
 * Sets the UFO's battlescape status.
 * @param inbattle .
 */
void Ufo::setInBattlescape(bool inbattle)
{
	_inBattlescape = inbattle;
}

/**
 * Returns the alien race currently residing in the UFO.
 * @return Alien race.
 */
std::string Ufo::getAlienRace() const
{
	return _race;
}

/**
 * Changes the alien race currently residing in the UFO.
 * @param race Alien race.
 */
void Ufo::setAlienRace(const std::string &race)
{
	_race = race;
}

void Ufo::setShotDownByCraftId(const int id)
{
	_shotDownByCraftId = id;
}

int Ufo::getShotDownByCraftId() const
{
	return _shotDownByCraftId;
}

/**
 * Returns a UFO's visibility to radar detection.
 * The UFO's size and altitude affect the chances
 * of it being detected by radars.
 * @return Visibility modifier.
 */
int Ufo::getVisibility() const
{
	int size = 0;
	// size = 15*(3-ufosize);
	if (_rules->getSize() == "STR_VERY_SMALL")
		size = -30;
	else if (_rules->getSize() == "STR_SMALL")
		size = -15;
	else if (_rules->getSize() == "STR_MEDIUM_UC")
		size = 0;
	else if (_rules->getSize() == "STR_LARGE")
		size = 15;
	else if (_rules->getSize() == "STR_VERY_LARGE")
		size = 30;

	int visibility = 0;
	if (_altitude == "STR_GROUND")
		visibility = -30;
	else if (_altitude == "STR_VERY_LOW")
		visibility = size - 20;
	else if (_altitude == "STR_LOW_UC")
		visibility = size - 10;
	else if (_altitude == "STR_HIGH_UC")
		visibility = size;
	else if (_altitude == "STR_VERY_HIGH")
		visibility = size - 10;

	return visibility;
}


/**
 * Returns the Mission of the UFO.
 * @return Mission.
 */
std::string Ufo::getMission() const
{
	return _mission;
}

/**
 * Changes the mission of the UFO.
 * @param mission Mission.
 */
void Ufo::setMission(const std::string &mission)
{
	_mission = mission;
}

/**
 * Returns whether this UFO has been detected by hyper-wave.
 * @return Detection status.
 */
bool Ufo::getHyperDetected() const
{
	return _hyperDetected;
}

/**
 * Changes whether this UFO has been detected by hyper-wave.
 * @param detected Detection status.
 */
void Ufo::setHyperDetected(bool hyperdetected)
{
	_hyperDetected = hyperdetected;
}

}
