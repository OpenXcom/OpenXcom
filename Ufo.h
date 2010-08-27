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

class RuleUfo;

/**
 * Represents an alien UFO on the map.
 * Contains variable info about a UFO like
 * position, damage, speed, etc.
 * @sa RuleUfo
 */
class Ufo
{
private:
	RuleUfo *_rules;
	double _lat, _lon;
	int _id, _damage, _speed, _altitude;
public:
	/// Creates a UFO of the specified type.
	Ufo(RuleUfo *rules);
	/// Cleans up the UFO.
	~Ufo();
	/// Gets the UFO's ruleset.
	RuleUfo *getRules();
	/// Gets the UFO's ID.
	int getId();
	/// Gets the UFO's latitude.
	double getLatitude();
	/// Sets the UFO's latitude.
	void setLatitude(double lat);
	/// Gets the UFO's longitude.
	double getLongitude();
	/// Sets the UFO's longitude.
	void setLongitude(double lon);
	/// Gets the UFO's amount of damage.
	int getDamage();
	/// Sets the UFO's amount of damage.
	void setDamage(int damage);
};

#endif
