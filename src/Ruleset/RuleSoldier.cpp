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
#include "RuleSoldier.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleunit for a certain
 * type of craft.
 * @param type String defining the type.
 */
RuleSoldier::RuleSoldier(const std::string &type) : _type(type), _armor(""), _standHeight(0), _kneelHeight(0), _loftemps(0)
{

}

/**
 *
 */
RuleSoldier::~RuleSoldier()
{

}

/**
 * Returns the language string that names
 * this unit. Each unit type has a unique name.
 * @return Unit name.
 */
std::string RuleSoldier::getType() const
{
	return _type;
}

void RuleSoldier::setStats(struct UnitStats minStats, struct UnitStats maxStats)
{
	_minStats = minStats;
	_maxStats = maxStats;
}


void RuleSoldier::setArmor(const std::string &armor)
{
	_armor = armor;
}

void RuleSoldier::setVoxelParameters(int standHeight, int kneelHeight, int loftemps)
{
	_standHeight = standHeight;
	_kneelHeight = kneelHeight;
	_loftemps = loftemps;
}

UnitStats RuleSoldier::getMinStats() const
{
	return _minStats;
}
///
UnitStats RuleSoldier::getMaxStats() const
{
	return _maxStats;
}
///
int RuleSoldier::getStandHeight() const
{
	return _standHeight;
}
///
int RuleSoldier::getKneelHeight() const
{
	return _kneelHeight;
}
///
int RuleSoldier::gotLoftemps() const
{
	return _loftemps;
}

std::string RuleSoldier::getArmor() const
{
	return _armor;
}

}
