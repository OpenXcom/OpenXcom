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
#ifndef OPENXCOM_RULEARMOR_H
#define OPENXCOM_RULEARMOR_H

#include <string>

namespace OpenXcom
{

/**
 * Represents a specific type of armor. (why the american spelling? - it's just shorter :)
 * Not only soldier armor, but also alien armor - some alien races wear Soldier Armor, Leader Armor or Commander Armor
 * depending on their rank.
 */
class RuleArmor
{
private:
	std::string _type, _spriteSheet, _corpseItem;
	int _frontArmor, _sideArmor, _rearArmor, _underArmor;

public:
	/// Creates a blank armor ruleset.
	RuleArmor(std::string type, std::string spriteSheet);
	/// Cleans up the armor ruleset.
	~RuleArmor();
	/// Gets the armor's type.
	std::string getType() const;
	/// Gets the unit's sprite sheet.
	std::string getSpriteSheet() const;
	/// Sets the different armor levels.
	void setArmor(int front, int side, int rear, int under);
	/// Get the front armor level.
	int getFrontArmor() const;
	/// Get the side armor level.
	int getSideArmor() const;
	/// get the rear armor level.
	int getRearArmor() const;
	/// get the under armor level.
	int getUnderArmor() const;
	/// Set the corpse item.
	void setCorpseItem(std::string corpseItem);
	/// Get the corpse item.
	std::string getCorpseItem() const;
};

}

#endif
