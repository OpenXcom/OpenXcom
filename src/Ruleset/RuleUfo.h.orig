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
#ifndef OPENXCOM_RULEUFO_H
#define OPENXCOM_RULEUFO_H

#include <string>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class RuleTerrain;
class Ruleset;


/**
 * Represents a specific type of UFO.
 * Contains constant info about a UFO like
 * speed, weapons, scores, etc.
 * @sa Ufo
 */
class RuleUfo
{
private:
	std::string _type, _size;
	int _sprite;
	int _damageMax, _speedMax, _accel, _power, _range, _score;
	RuleTerrain *_battlescapeTerrainData;
public:
	/// Creates a blank UFO ruleset.
	RuleUfo(const std::string &type);
	/// Cleans up the UFO ruleset.
	~RuleUfo();
	/// Loads UFO data from YAML.
	void load(const YAML::Node& node, Ruleset *ruleset);
	/// Saves the UFO data to YAML.
	void save(YAML::Emitter& out) const;
	/// Gets the UFO's type.
	std::string getType() const;
	/// Gets the UFO's size.
	std::string getSize() const;
	/// Sets the UFO's size.
	void setSize(const std::string &size);
	/// Gets the UFO's radius.
	int getRadius() const;
	/// Gets the UFO's sprite.
	int getSprite() const;
	/// Sets the UFO's sprite.
	void setSprite(int sprite);
	/// Gets the UFO's maximum damage.
	int getMaxDamage() const;
	/// Sets the UFO's maximum damage.
	void setMaxDamage(int damage);
	/// Gets the UFO's maximum speed.
	int getMaxSpeed() const;
	/// Sets the UFO's maximum speed.
	void setMaxSpeed(int speed);
	/// Gets the UFO's acceleration.
	int getAcceleration() const;
	/// Sets the UFO's acceleration.
	void setAcceleration(int accel);
	/// Gets the UFO's weapon power.
	int getWeaponPower() const;
	/// Sets the UFO's weapon power.
	void setWeaponPower(int power);
	/// Gets the UFO's weapon range.
	int getWeaponRange() const;
	/// Sets the UFO's weapon range.
	void setWeaponRange(int range);
	/// Gets the UFO's score.
	int getScore() const;
	/// Sets the UFO's score.
	void setScore(int score);
	/// Sets the battlescape terrain data ruleset for this UFO
	RuleTerrain *getBattlescapeTerrainData();
	/// Gets the battlescape terrain data ruleset for this UFO
	void setBattlescapeTerrainData(RuleTerrain *t);
};

}

#endif
