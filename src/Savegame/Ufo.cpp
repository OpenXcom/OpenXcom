/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include <assert.h>
#include <algorithm>
#include <functional>
#include "../fmath.h"
#include "Craft.h"
#include "AlienMission.h"
#include "../Engine/Exception.h"
#include "../Engine/Language.h"
#include "../Mod/Mod.h"
#include "../Mod/RuleUfo.h"
#include "../Mod/UfoTrajectory.h"
#include "../Mod/RuleAlienMission.h"
#include "SavedGame.h"
#include "Waypoint.h"

namespace OpenXcom
{

const char *Ufo::ALTITUDE_STRING[] = {
	"STR_GROUND",
	"STR_VERY_LOW",
	"STR_LOW_UC",
	"STR_HIGH_UC",
	"STR_VERY_HIGH"
};

/**
 * Initializes a UFO of the specified type.
 * @param rules Pointer to ruleset.
 */
Ufo::Ufo(const RuleUfo *rules)
  : MovingTarget(), _rules(rules), _crashId(0), _landId(0), _damage(0), _direction("STR_NORTH")
  , _altitude("STR_HIGH_UC"), _status(FLYING), _secondsRemaining(0)
  , _inBattlescape(false), _mission(0), _trajectory(0)
  , _trajectoryPoint(0), _detected(false), _hyperDetected(false), _processedIntercept(false), _shootingAt(0), _hitFrame(0)
  , _fireCountdown(0), _escapeCountdown(0)
{
}

/**
 * Make sure our mission forget's us, and we only delete targets we own (waypoints).
 *
 */
Ufo::~Ufo()
{
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
 * @param mod The game mod. Use to access the trajectory rules.
 * @param game The game data. Used to find the UFO's mission.
 */
void Ufo::load(const YAML::Node &node, const Mod &mod, SavedGame &game)
{
	MovingTarget::load(node);
	_crashId = node["crashId"].as<int>(_crashId);
	_landId = node["landId"].as<int>(_landId);
	_damage = node["damage"].as<int>(_damage);
	_altitude = node["altitude"].as<std::string>(_altitude);
	_direction = node["direction"].as<std::string>(_direction);
	_detected = node["detected"].as<bool>(_detected);
	_hyperDetected = node["hyperDetected"].as<bool>(_hyperDetected);
	_secondsRemaining = node["secondsRemaining"].as<size_t>(_secondsRemaining);
	_inBattlescape = node["inBattlescape"].as<bool>(_inBattlescape);
	double lon = _lon;
	double lat = _lat;
	if (const YAML::Node &dest = node["dest"])
	{
		lon = dest["lon"].as<double>();
		lat = dest["lat"].as<double>();
	}
	_dest = new Waypoint();
	_dest->setLongitude(lon);
	_dest->setLatitude(lat);
	if (const YAML::Node &status = node["status"])
	{
		_status = (UfoStatus)status.as<int>();
	}
	else
	{
		if (isDestroyed())
		{
			_status = DESTROYED;
		}
		else if (isCrashed())
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
	if (game.getMonthsPassed() != -1)
	{
		int missionID = node["mission"].as<int>();
		std::vector<AlienMission *>::const_iterator found = std::find_if (game.getAlienMissions().begin(), game.getAlienMissions().end(), matchMissionID(missionID));
		if (found == game.getAlienMissions().end())
		{
			// Corrupt save file.
			throw Exception("Unknown UFO mission, save file is corrupt.");
		}
		_mission = *found;

		std::string tid = node["trajectory"].as<std::string>();
		_trajectory = mod.getUfoTrajectory(tid);
		if (_trajectory == 0)
		{
			// Corrupt save file.
			throw Exception("Unknown UFO trajectory, save file is corrupt.");
		}
		_trajectoryPoint = node["trajectoryPoint"].as<size_t>(_trajectoryPoint);
	}
	_fireCountdown = node["fireCountdown"].as<int>(_fireCountdown);
	_escapeCountdown = node["escapeCountdown"].as<int>(_escapeCountdown);
	if (_inBattlescape)
		setSpeed(0);
}

/**
 * Saves the UFO to a YAML file.
 * @return YAML node.
 */
YAML::Node Ufo::save(bool newBattle) const
{
	YAML::Node node = MovingTarget::save();
	node["type"] = _rules->getType();
	if (_crashId)
	{
		node["crashId"] = _crashId;
	}
	else if (_landId)
	{
		node["landId"] = _landId;
	}
	node["damage"] = _damage;
	node["altitude"] = _altitude;
	node["direction"] = _direction;
	node["status"] = (int)_status;
	if (_detected)
		node["detected"] = _detected;
	if (_hyperDetected)
		node["hyperDetected"] = _hyperDetected;
	if (_secondsRemaining)
		node["secondsRemaining"] = _secondsRemaining;
	if (_inBattlescape)
		node["inBattlescape"] = _inBattlescape;
	if (!newBattle)
	{
		node["mission"] = _mission->getId();
		node["trajectory"] = _trajectory->getID();
		node["trajectoryPoint"] = _trajectoryPoint;
	}

	node["fireCountdown"] = _fireCountdown;
	node["escapeCountdown"] = _escapeCountdown;
	return node;
}

/**
 * Returns the UFO's unique type used for
 * savegame purposes.
 * @return ID.
 */
std::string Ufo::getType() const
{
	return "STR_UFO";
}

/**
 * Returns the ruleset for the UFO's type.
 * @return Pointer to ruleset.
 */
const RuleUfo *Ufo::getRules() const
{
	return _rules;
}

/**
 * Changes the ruleset for the UFO's type.
 * @param rules Pointer to ruleset.
 * @warning ONLY FOR NEW BATTLE USE!
 */
void Ufo::changeRules(const RuleUfo *rules)
{
	_rules = rules;
}

/**
 * Returns the UFO's unique default name.
 * @param lang Language to get strings from.
 * @return Full name.
 */
std::string Ufo::getDefaultName(Language *lang) const
{
	switch (_status)
	{
	case LANDED:
		return lang->getString(getMarkerName()).arg(_landId);
	case CRASHED:
		return lang->getString(getMarkerName()).arg(_crashId);
	default:
		return lang->getString(getMarkerName()).arg(_id);
	}
}

/**
 * Returns the name on the globe for the UFO.
 * @return String ID.
 */
std::string Ufo::getMarkerName() const
{
	switch (_status)
	{
	case LANDED:
		return "STR_LANDING_SITE_";
	case CRASHED:
		return "STR_CRASH_SITE_";
	default:
		return "STR_UFO_";
	}
}

/**
 * Returns the marker ID on the globe for the UFO.
 * @return Marker ID.
 */
int Ufo::getMarkerId() const
{
	switch (_status)
	{
	case LANDED:
		return _landId;
	case CRASHED:
		return _crashId;
	default:
		return _id;
	}
}

/**
 * Returns the globe marker for the UFO.
 * @return Marker sprite, -1 if none.
 */
int Ufo::getMarker() const
{
	if (!_detected)
		return -1;
	switch (_status)
	{
	case LANDED:
		return _rules->getLandMarker() == -1 ? 3 : _rules->getLandMarker();
	case CRASHED:
		return _rules->getCrashMarker() == -1 ? 4 : _rules->getCrashMarker();
	default:
		return _rules->getMarker() == -1 ? 2 : _rules->getMarker();
	}
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
	if (isDestroyed())
	{
		_status = DESTROYED;
	}
	else if (isCrashed())
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
size_t Ufo::getSecondsRemaining() const
{
	return _secondsRemaining;
}

/**
 * Changes the amount of remaining seconds the UFO has left on the ground.
 * After this many seconds thet UFO will take off, if landed, or disappear, if
 * crashed.
 * @param seconds Amount of seconds.
 */
void Ufo::setSecondsRemaining(size_t seconds)
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
 * @return Altitude as string ID.
 */
std::string Ufo::getAltitude() const
{
	return _altitude;
}

/**
 * Returns the current altitude of the UFO.
 * @return Altitude as integer (0-4).
 */
int Ufo::getAltitudeInt() const
{
	for (size_t i = 0; i < 5; ++i)
	{
		if (ALTITUDE_STRING[i] == _altitude)
		{
			return i;
		}
	}
	return -1;
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

	double x = _speedLon;
	double y = -_speedLat;

	// This section guards vs. divide-by-zero.
	if (AreSame(x, 0.0) || AreSame(y, 0.0))
	{
		if (AreSame(x, 0.0) && AreSame(y, 0.0))
		{
			_direction = "STR_NONE_UC";
		}
		else if (AreSame(x, 0.0))
		{
			if (y > 0.f)
			{
				_direction = "STR_NORTH";
			}
			else if (y < 0.f)
			{
				_direction = "STR_SOUTH";
			}
		}
		else if (AreSame(y, 0.0))
		{
			if (x > 0.f)
			{
				_direction = "STR_EAST";
			}
			else if (x < 0.f)
			{
				_direction = "STR_WEST";
			}
		}

		return;
	}

	double theta = atan2(y, x); // radians
	theta = theta * 180.f / M_PI; // +/- 180 deg.

	if (22.5f > theta && theta > -22.5f)
	{
		_direction = "STR_EAST";
	}
	else if (-22.5f > theta && theta > -67.5f)
	{
		_direction = "STR_SOUTH_EAST";
	}
	else if (-67.5f > theta && theta > -112.5f)
	{
		_direction = "STR_SOUTH";
	}
	else if (-112.5f > theta && theta > -157.5f)
	{
		_direction = "STR_SOUTH_WEST";
	}
	else if (-157.5f > theta || theta > 157.5f)
	{
		_direction = "STR_WEST";
	}
	else if (157.5f > theta && theta > 112.5f)
	{
		_direction = "STR_NORTH_WEST";
	}
	else if (112.5f > theta && theta > 67.5f)
	{
		_direction = "STR_NORTH";
	}
	else
	{
		_direction = "STR_NORTH_EAST";
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
 * @return Is the UFO currently in battle?
 */
bool Ufo::isInBattlescape() const
{
	return _inBattlescape;
}

/**
 * Sets the UFO's battlescape status.
 * @param inbattle True if it's in battle, False otherwise.
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

void Ufo::setShotDownByCraftId(const CraftId& craft)
{
	_shotDownByCraftId = craft;
}

CraftId Ufo::getShotDownByCraftId() const
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
	return _mission->getRules().getType();
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
 * @param hyperdetected Detection status.
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

/**
 * Gets which interception window the UFO is active in.
 * @return which interception window the UFO is active in.
 */
int Ufo::getShootingAt() const
{
	return _shootingAt;
}

/**
 * Sets which interception window the UFO is active in.
 * @param target the window the UFO is active in.
 */
void Ufo::setShootingAt(int target)
{
	_shootingAt = target;
}

/**
 * Gets the UFO's landing site ID.
 * @return landing site ID.
 */
int Ufo::getLandId() const
{
	return _landId;
}

/**
 * Sets the UFO's landing site ID.
 * @param id landing site ID.
 */
void Ufo::setLandId(int id)
{
	_landId = id;
}

/**
 * Gets the UFO's crash site ID.
 * @return the UFO's crash site ID.
 */
int Ufo::getCrashId() const
{
	return _crashId;
}

/**
 * Sets the UFO's crash site ID.
 * @param id the UFO's crash site ID.
 */
void Ufo::setCrashId(int id)
{
	_crashId = id;
}

/**
 * Sets the UFO's hit frame.
 * @param frame the hit frame.
 */
void Ufo::setHitFrame(int frame)
{
	_hitFrame = frame;
}

/**
 * Gets the UFO's hit frame.
 * @return the hit frame.
 */
int Ufo::getHitFrame() const
{
	return _hitFrame;
}

/**
 * Sets the countdown timer for escaping a dogfight.
 * @param time how many ticks until the ship attempts to escape.
 */
void Ufo::setEscapeCountdown(int time)
{
	_escapeCountdown = time;
}

/**
 * Gets the escape timer for dogfights.
 * @return how many ticks until the ship tries to leave.
 */
int Ufo::getEscapeCountdown() const
{
	return _escapeCountdown;
}

/**
 * Sets the number of ticks until the ufo fires its weapon.
 * @param time number of ticks until refire.
 */
void Ufo::setFireCountdown(int time)
{
	_fireCountdown = time;
}

/**
 * Gets the number of ticks until the ufo is ready to fire.
 * @return ticks until weapon is ready.
 */
int Ufo::getFireCountdown() const
{
	return _fireCountdown;
}

/**
 * Sets a flag denoting that this ufo has had its timers decremented.
 * prevents multiple interceptions from decrementing or resetting an already running timer.
 * this flag is reset in advance each time the geoscape processes the dogfights.
 * @param processed whether or not we've had our timers processed.
 */
void Ufo::setInterceptionProcessed(bool processed)
{
	_processedIntercept = processed;
}

/**
 * Gets if the ufo has had its timers decremented on this cycle of interception updates.
 * @return if this ufo has already been processed.
 */
bool Ufo::getInterceptionProcessed() const
{
	return _processedIntercept;
}

}
