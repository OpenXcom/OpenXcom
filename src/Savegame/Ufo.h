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
#ifndef OPENXCOM_UFO_H
#define OPENXCOM_UFO_H

#include "MovingTarget.h"
#include <string>
#include "yaml.h"

namespace OpenXcom
{

class RuleUfo;

/**
 * Represents an alien UFO on the map.
 * Contains variable info about a UFO like
 * position, damage, speed, etc.
 * @sa RuleUfo
 */
class Ufo : public MovingTarget
{
private:
	RuleUfo *_rules;
	int _id, _damage, _altitude;
	std::string _direction;
	bool _detected;
	int _daysCrashed;
	bool _inBattlescape;
	
	/// Calculates a new speed vector to the destination.
	void calculateSpeed();
public:
	/// Creates a UFO of the specified type.
	Ufo(RuleUfo *rules);
	/// Cleans up the UFO.
	~Ufo();
	/// Loads the UFO from YAML.
	void load(const YAML::Node& node);
	/// Saves the UFO to YAML.
	void save(YAML::Emitter& out) const;
	/// Saves the UFO's ID to YAML.
	void saveId(YAML::Emitter& out) const;
	/// Gets the UFO's ruleset.
	RuleUfo *const getRules() const;
	/// Gets the UFO's ID.
	int getId() const;
	/// Sets the UFO's ID.
	void setId(int id);
	/// Gets the UFO's name.
	std::string getName(Language *lang) const;
	/// Gets the UFO's amount of damage.
	int getDamage() const;
	/// Sets the UFO's amount of damage.
	void setDamage(int damage);
	/// Gets the UFO's detection status.
	bool getDetected() const;
	/// Sets the UFO's detection status.
	void setDetected(bool detected);
	/// Gets the UFO's amount of crashed days.
	int getDaysCrashed() const;
	/// Sets the UFO's amount of crashed days.
	void setDaysCrashed(int days);
	/// Gets the UFO's direction.
	std::string getDirection() const;
	/// Gets the UFO's altitude.
	std::string getAltitude() const;
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
};

}

#endif
