/*
 * Copyright 2010 Daniel Albano
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
#ifndef OPENXCOM__UFO_H
#define OPENXCOM__UFO_H

#include "MovingTarget.h"
#include <string>
#include "LangString.h"

class RuleUfo;

using namespace std;

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
	LangString _direction;
	bool _detected;
	int _daysCrashed;
public:
	/// Creates a UFO of the specified type.
	Ufo(RuleUfo *rules);
	/// Cleans up the UFO.
	~Ufo();
	/// Gets the UFO's ruleset.
	RuleUfo *getRules();
	/// Gets the UFO's ID.
	int getId();
	/// Sets the UFO's ID.
	void setId(int id);
	/// Gets the UFO's name.
	string getName(Language *lang);
	/// Gets the UFO's amount of damage.
	int getDamage();
	/// Sets the UFO's amount of damage.
	void setDamage(int damage);
	/// Gets the UFO's detection status.
	bool getDetected();
	/// Sets the UFO's detection status.
	void setDetected(bool detected);
	/// Gets the UFO's amount of crashed days.
	int getDaysCrashed();
	/// Sets the UFO's amount of crashed days.
	void setDaysCrashed(int days);
	/// Gets the UFO's direction.
	LangString getDirection();
	/// Gets the UFO's altitude.
	LangString getAltitude();
	/// Gets if the UFO has crashed.
	bool isCrashed();
	/// Gets if the UFO has been destroyed.
	bool isDestroyed();
	/// Calculates a new speed vector to the destination.
	void calculateSpeed();
	/// Handles UFO logic.
	void think();
};

#endif
