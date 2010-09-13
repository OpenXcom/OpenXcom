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
#ifndef OPENXCOM__RULEUFO_H
#define OPENXCOM__RULEUFO_H

#include "LangString.h"

/**
 * Represents a specific type of UFO.
 * Contains constant info about a UFO like
 * speed, weapons, scores, etc.
 * @sa Ufo
 */
class RuleUfo
{
private:
	LangString _type, _size;
	int _damageMax, _speedMax, _accel, _power, _range, _score;
public:
	/// Creates a blank UFO ruleset.
	RuleUfo(LangString type);
	/// Cleans up the UFO ruleset.
	~RuleUfo();
	/// Gets the UFO's type.
	LangString getType();
	/// Gets the UFO's size.
	LangString getSize();
	/// Sets the UFO's size.
	void setSize(LangString size);
	/// Gets the UFO's radius.
	int getRadius();
	/// Gets the UFO's maximum damage.
	int getMaxDamage();
	/// Sets the UFO's maximum damage.
	void setMaxDamage(int damage);
	/// Gets the UFO's maximum speed.
	int getMaxSpeed();
	/// Sets the UFO's maximum speed.
	void setMaxSpeed(int speed);
	/// Gets the UFO's acceleration.
	int getAcceleration();
	/// Sets the UFO's acceleration.
	void setAcceleration(int accel);
	/// Gets the UFO's weapon power.
	int getWeaponPower();
	/// Sets the UFO's weapon power.
	void setWeaponPower(int power);
	/// Gets the UFO's weapon range.
	int getWeaponRange();
	/// Sets the UFO's weapon range.
	void setWeaponRange(int range);
	/// Gets the UFO's score.
	int getScore();
	/// Sets the UFO's score.
	void setScore(int score);
};

#endif
