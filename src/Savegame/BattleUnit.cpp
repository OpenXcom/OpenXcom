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
#include "BattleUnit.h"
#include "../Engine/Palette.h"

/**
 * Initializes a BattleUnit.
 */
BattleUnit::BattleUnit(RuleUnitSprite *renderRules) : _renderRules(renderRules), _id(0), _pos(Position()), _direction(0)
{
}

/**
 *
 */
BattleUnit::~BattleUnit()
{
	
}

/**
 * Returns the BattleUnit's unique ID.
 * @return Unique ID.
 */
int BattleUnit::getId() const
{
	return _id;
}

/**
 * Changes the BattleUnit's unique ID.
 * @param id Unique ID.
 */
void BattleUnit::setId(int id)
{
	_id = id;
}

/**
 * Returns the ruleset for the unit's type.
 * @return Pointer to ruleset.
 */
RuleUnitSprite *const BattleUnit::getRenderRules() const
{
	return _renderRules;
}

/**
 * Changes the BattleUnit's position.
 * @param pos position
 */
void BattleUnit::setPosition(const Position& pos)
{
	_pos = pos;
}

/**
 * Changes the BattleUnit's position.
 * @return pointer to position
 */
const Position& BattleUnit::getPosition() const
{
	return _pos;
}

/**
 * Changes the BattleUnit's direction.
 * @param direction
 */
void BattleUnit::setDirection(int direction)
{
	_direction = direction;
}

/**
 * Changes the BattleUnit's direction.
 * @return direction
 */
int BattleUnit::getDirection() const
{
	return _direction;
}