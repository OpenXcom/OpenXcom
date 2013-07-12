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
#ifndef OPENXCOM_UFO_H
#define OPENXCOM_UFO_H

#include "MovingTarget.h"
#include <string>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class RuleUfo;
class AlienMission;
class UfoTrajectory;
class SavedGame;
class Ruleset;

/**
 * Represents an alien UFO on the map.
 * Contains variable info about a UFO like
 * position, damage, speed, etc.
 * @sa RuleUfo
 */
class Ufo : public MovingTarget
{
public:
	enum UfoStatus { FLYING, LANDED, CRASHED, DESTROYED };
private:
	RuleUfo *_rules;
	int _id, _crashId, _landId, _damage;
	std::string _direction, _altitude;
	enum UfoStatus _status;
	unsigned _secondsRemaining;
	bool _inBattlescape;
	int _shotDownByCraftId;
	AlienMission *_mission;
	const UfoTrajectory *_trajectory;
	unsigned _trajectoryPoint;
	bool _detected, _hyperDetected;
	int _shootingAt;
	/// Calculates a new speed vector to the destination.
	void calculateSpeed();
public:
	/// Creates a UFO of the specified type.
	Ufo(RuleUfo *rules);
	/// Cleans up the UFO.
	~Ufo();
	/// Loads the UFO from YAML.
	void load(const YAML::Node& node, const Ruleset &ruleset, SavedGame &game);
	/// Saves the UFO to YAML.
	void save(YAML::Emitter& out) const;
	/// Saves the UFO's ID to YAML.
	void saveId(YAML::Emitter& out) const;
	/// Gets the UFO's ruleset.
	RuleUfo *getRules() const;
	/// Gets the UFO's ID.
	int getId() const;
	/// Sets the UFO's ID.
	void setId(int id);
	/// Gets the UFO's name.
	std::wstring getName(Language *lang) const;
	/// Gets the UFO's amount of damage.
	int getDamage() const;
	/// Sets the UFO's amount of damage.
	void setDamage(int damage);
	/// Gets the UFO's detection status.
	bool getDetected() const;
	/// Sets the UFO's detection status.
	void setDetected(bool detected);
	/// Gets the UFO's seconds left on the ground.
	int getSecondsRemaining() const;
	/// Sets the UFO's seconds left on the ground.
	void setSecondsRemaining(int seconds);
	/// Gets the UFO's direction.
	std::string getDirection() const;
	/// Gets the UFO's altitude.
	std::string getAltitude() const;
	/// Sets the UFO's altitude.
	void setAltitude(const std::string &altitude);
	/// Gets the UFO status
	enum UfoStatus getStatus() const { return _status; }
	/// Set the UFO's status.
	void setStatus(enum UfoStatus status) {_status = status; }
	/// Gets if the UFO has crashed.
	bool isCrashed() const;
	/// Gets if the UFO has been destroyed.
	bool isDestroyed() const;
	/// Handles UFO logic.
	void think();
	/// Sets the UFO's battlescape status.
	void setInBattlescape(bool inbattle);
	/// Gets if the UFO is in battlescape.
	bool isInBattlescape() const;
	/// Gets the UFO's alien race.
	const std::string &getAlienRace() const;
	/// Sets the ID of craft which shot down the UFO.
	void setShotDownByCraftId(const int id);
	/// Gets the ID of craft which shot down the UFO.
	int getShotDownByCraftId() const;
	/// Gets the UFO's visibility.
	int getVisibility() const;
	/// Gets the UFO's Mission type.
	const std::string &getMissionType() const;
	/// Sets the UFO's mission information.
	void setMissionInfo(AlienMission *mission, const UfoTrajectory *trajectory);
	/// Gets the UFO's hyper detection status.
	bool getHyperDetected() const;
	/// Sets the UFO's hyper detection status.
	void setHyperDetected(bool hyperdetected);
	/// Gets the UFO's progress on the trajectory track.
	unsigned getTrajectoryPoint() const { return _trajectoryPoint; }
	/// Sets the UFO's progress on the trajectory track.
	void setTrajectoryPoint(unsigned np) { _trajectoryPoint = np; }
	/// Gets the UFO's trajectory.
	const UfoTrajectory &getTrajectory() const { return *_trajectory; }
	/// Gets the UFO's mission object.
	AlienMission *getMission() const { return _mission; }
	/// Sets the UFO's destination.
	void setDestination(Target *dest);
	/// Get which interceptor this ufo is engaging.
	int getShootingAt() const;
	/// Set which interceptor this ufo is engaging.
	void setShootingAt(int target);
	/// Gets the UFO's landing site ID.
	int getLandId() const;
	/// Sets the UFO's landing site ID.
	void setLandId(int id);
	/// Gets the UFO's crash site ID.
	int getCrashId() const;
	/// Sets the UFO's crash site ID.
	void setCrashId(int id);

};

}

#endif
