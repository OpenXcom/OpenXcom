/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include "SoldierDeath.h"
#include "BattleUnitStatistics.h"

namespace OpenXcom
{

/**
 * Initializes a death event.
 */
SoldierDeath::SoldierDeath() : _time(0,0,0,0,0,0,0), _cause(0)
{
}

SoldierDeath::~SoldierDeath()
{
	delete _cause;
}

/**
 * Loads the death from a YAML file.
 * @param node YAML node.
 */
void SoldierDeath::load(const YAML::Node &node)
{
	_time.load(node["time"]);
	if (node["cause"])
	{
		_cause = new BattleUnitKills();
		_cause->load(node["cause"]);
	}
}

/**
 * Saves the death to a YAML file.
 * @returns YAML node.
 */
YAML::Node SoldierDeath::save() const
{
	YAML::Node node;
	node["time"] = _time.save();
	if (_cause != 0)
	{
		node["cause"] = _cause->save();
	}
	return node;
}

/**
 * Returns the time of death of this soldier.
 * @return Pointer to the time.
 */
const GameTime *SoldierDeath::getTime() const
{
	return &_time;
}

/**
 * Changes the time of death of this soldier.
 * @param time The time of death.
 */
void SoldierDeath::setTime(GameTime time)
{
	_time = time;
}

/**
* Returns the time of death of this soldier.
* @return Pointer to the time.
*/
const BattleUnitKills *SoldierDeath::getCause() const
{
	return _cause;
}

/**
* Changes the time of death of this soldier.
* @param time The time of death.
*/
void SoldierDeath::setCause(BattleUnitKills *cause)
{
	_cause = cause;
}

}
