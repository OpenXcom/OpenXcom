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
#ifndef OPENXCOM_BATTLEUNIT_H
#define OPENXCOM_BATTLEUNIT_H

class RuleUnitSprite;
class Position;

/**
 * Represents a moving unit in the battlescape, player controlled or AI controlled
 * it holds info about it's position, items carrying, stats, etc
 */
class BattleUnit
{
private:
	RuleUnitSprite *_renderRules;
	int _id;
	Position *_pos;
	int _direction;
public:
	/// Creates a BattleUnit.
	BattleUnit(RuleUnitSprite *renderRules);
	/// Cleans up the BattleUnit.
	~BattleUnit();
	/// Gets the BattleUnit's ID.
	int getId();
	/// Sets the BattleUnit's ID.
	void setId(int id);
	/// Gets the unit's rendering ruleset.
	RuleUnitSprite *const getRenderRules() const;
	/// Sets the unit's position X, Y, Z
	void setPosition(Position *pos);
	/// gets the unit's position
	Position *getPosition();
	/// Sets the unit's direction 0-7
	void setDirection(int direction);
	/// gets the unit's direction
	int getDirection();

};

#endif
