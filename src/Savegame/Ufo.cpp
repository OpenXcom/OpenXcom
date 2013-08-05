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
#include "Ufo.h"
#include "Craft.h"
#include "AlienMission.h"
#include "../Engine/Exception.h"
#include "../Engine/Language.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleUfo.h"
#include "../Ruleset/UfoTrajectory.h"
#include "SavedGame.h"
#include "Waypoint.h"
#include <assert.h>
#include <cmath>
#include <sstream>
#include <algorithm>

namespace OpenXcom
{

/**
 * Initializes a UFO of the specified type.
 * @param rules Pointer to ruleset.
 */
Ufo::Ufo(RuleUfo *rules)
  : MovingTarget(), _rules(rules), _id(0), _crashId(0), _landId(0), _damage(0), _direction("STR_NORTH")
  , _altitude("STR_HIGH_UC"), _status(FLYING), _secondsRemaining(0)
  , _inBattlescape(false), _shotDownByCraftId(-1), _mission(0), _trajectory(0)
  , _detected(false), _hyperDetected(false), _shootingAt(0)
{
}

/**
 * Make sure our mission forget's us, and we only delete targets we own (waypoints).
 *
 */
Ufo::~Ufo()
{
	for (std::vector<Target*>::iterator i = _followers.begin(); i != _followers.end();)
	{
		Craft *c = dynamic_cast<Craft*>(*i);
		if (c)
		{
			c->returnToBase();
			i = _followers.begin();
		}
		else
		{
			 ++i;
		}
	}
	if (_mission)
	{
		_mission->decreaseLiveUfos();
	}
	if (_dest)
	{
		Waypoint *wp = dynamic_cast<Waypoint*>(_dest);
		if (wp != 0)
		{
			delete _dest;
			_dest = 0;
		}
	}
}

/**
 * Match AlienMission based on the unique ID.
 */
class matchMissionID: public std::unary_function<const AlienMission *, bool>
{
public:
	/// Store ID for later comparisons.
	matchMissionID(int id) : _id(id) { /* Empty by design. */ }
	/// Match with stored ID.
	bool operator()(const AlienMission *am) const { return am->getId() == _id; }
private:
	int _id;
};

/**
 * Loads the UFO from a YAML file.
 * @param node YAML node.
 * @param ruleset The game rules. Use to access the trajectory rules.
 * @param game The game data. Used to find the UFO's mission.
 */
void Ufo::load(const YAML::Node &node, const Ruleset &ruleset, SavedGame &game)
{
	MovingTarget::load(node);
	node["id"] >> _id;
	if (const YAML::Node *crashId = node.FindValue("crashId"))
	{
		*crashId >> _crashId;
	}
	else if (const YAML::Node *landId = node.FindValue("landId"))
	{
		*landId >> _landId;
	}
	node["damage"] >> _damage;
	node["altitude"] >> _altitude;
	node["direction"] >> _direction;
	node["detected"] >> _detected;
	node["hyperDetected"] >> _hyperDetected;
	node["secondsRemaining"] >> _secondsRemaining;
	node["inBattlescape"] >> _inBattlescape;
	double lon = _lon;
	double lat = _lat;
	if (const YAML::Node *dest = node.FindValue("dest"))
	{
		(*dest)["lon"] >> lon;
		(*dest)["lat"] >> lat;
	}
	_dest = new Waypoint();
	_dest->setLongitude(lon);
	_dest->setLatitude(lat);
	if (const YAML::Node *status = node.FindValue("status"))
	{
		int a;
		(*status) >> a;
		_status = (UfoStatus)a;
	}
	else
	{
		if (_damage >= _rules->getMaxDamage())
		{
			_status = DESTROYED;
		}
		else if (_damage >= _rules->getMaxDamage() / 2)
		{
			_status = CRASHED;
		}
		else if (_altitude == "STR_GROUND")
		{
			_status = LANDED;
		}
		else
		{
			_status = FLYING;
		}
	}
	int missionID;
	node["mission"] >> missionID;
	std::vector<AlienMission *>::const_iterator found = std::find_if(game.getAlienMissions().begin(), game.getAlienMissions().end(), matchMissionID(missionID));
	if (found == game.getAlienMissions().end())
	{
		// Corrupt save file.
		throw Exception("Unknown mission, save file is corrupt.");
	}
	_mission = *found;

	std::string tid;
	node["trajectory"] >> tid;
	_trajectory = ruleset.getUfoTrajectory(tid);
	node["trajectoryPoint"] >> _trajectoryPoint;
	if (_inBattlescape)
		setSpeed(0);
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
	if (_crashId)
	{
		out << YAML::Key << "crashId" << YAML::Value << _crashId;
	}
	else if (_landId)
	{
		out << YAML::Key << "landId" << YAML::Value << _landId;
	}
	out << YAML::Key << "damage" << YAML::Value << _damage;
	out << YAML::Key << "altitude" << YAML::Value << _altitude;
	out << YAML::Key << "direction" << YAML::Value << _direction;
	out << YAML::Key << "status" << YAML::Value << _status;
	out << YAML::Key << "detected" << YAML::Value << _detected;
	out << YAML::Key << "hyperDetected" << YAML::Value << _hyperDetected;
	out << YAML::Key << "secondsRemaining" << YAML::Value << _secondsRemaining;
	out << YAML::Key << "inBattlescape" << YAML::Value << _inBattlescape;
	out << YAML::Key << "mission" << YAML::Value << _mission->getId();
	out << YAML::Key << "trajectory" << YAML::Value << _trajectory->getID();
	out << YAML::Key << "trajectoryPoint" << YAML::Value << _trajectoryPoint;
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
	case DESTROYED: // Destroyed also means leaving Earth.
		name << lang->getString("STR_UFO_") << _id;
		break;
	case LANDED:
		name << lang->getString("STR_LANDING_SITE_") << _landId;
		break;
	case CRASHED:
		name << lang->getString("STR_CRASH_SITE_") << _crashId;
		break;
	}
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
 * Returns the amount of remaining seconds the UFO has left on the ground.
 * After this many seconds thet UFO will take off, if landed, or disappear, if
 * crashed.
 * @return Amount of seconds.
 */
int Ufo::getSecondsRemaining() const
{
	return _secondsRemaining;
}

/**
 * Changes the amount of remaining seconds the UFO has left on the ground.
 * After this many seconds thet UFO will take off, if landed, or disappear, if
 * crashed.
 * @param seconds Amount of seconds.
 */
void Ufo::setSecondsRemaining(int seconds)
{
	_secondsRemaining = seconds;
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
	return (_damage > _rules->getMaxDamage() / 2);
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
			// Prevent further movement.
			setSpeed(0);
		}
		break;
	case LANDED:
		assert(_secondsRemaining >= 5 && "Wrong time management.");
		_secondsRemaining -= 5;
		break;
	case CRASHED:
		if (!_detected)
		{
			_detected = true;
		}
		// This gets handled in GeoscapeState::time30Minutes()
		// Because the original game processes it every 30 minutes!
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
	if (inbattle)
		setSpeed(0);
	_inBattlescape = inbattle;
}

/**
 * Returns the alien race currently residing in the UFO.
 * @return Alien race.
 */
const std::string &Ufo::getAlienRace() const
{
	return _mission->getRace();
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
 * Returns the Mission type of the UFO.
 * @return Mission.
 */
const std::string &Ufo::getMissionType() const
{
	return _mission->getType();
}

/**
 * Sets the mission information of the UFO.
 * The UFO will start at the first point of the trajectory. The actual UFO
 * information is not changed here, this only sets the information kept on
 * behalf of the mission.
 * @param mission Pointer to the actual mission object.
 * @param trajectory Pointer to the actual mission trajectory.
 */
void Ufo::setMissionInfo(AlienMission *mission, const UfoTrajectory *trajectory)
{
	assert(!_mission && mission && trajectory);
	_mission = mission;
	_mission->increaseLiveUfos();
	_trajectoryPoint = 0;
	_trajectory = trajectory;
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

/**
 * Handle destination changes, making sure to delete old waypoint destinations.
 * @param dest Pointer to the new destination.
 */
void Ufo::setDestination(Target *dest)
{
	Waypoint *old = dynamic_cast<Waypoint*>(_dest);
	MovingTarget::setDestination(dest);
	delete old;
}

int Ufo::getShootingAt() const
{
	return _shootingAt;
}

void Ufo::setShootingAt(int target)
{
	_shootingAt = target;
}
/// Gets the UFO's landing site ID.
int Ufo::getLandId() const
{
	return _landId;
}
/// Sets the UFO's landing site ID.
void Ufo::setLandId(int id)
{
	_landId = id;
}
/// Gets the UFO's crash site ID.
int Ufo::getCrashId() const
{
	return _crashId;
}
/// Sets the UFO's crash site ID.
void Ufo::setCrashId(int id)
{
	_crashId = id;
}
}
