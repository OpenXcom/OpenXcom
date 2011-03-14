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
#include "RuleAlien.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleunit for a certain
 * type of craft.
 * @param type String defining the type.
 */
RuleAlien::RuleAlien(std::string type, std::string race, std::string rank) : _type(type), _race(race), _rank(rank)
{
	
}

/**
 *
 */
RuleAlien::~RuleAlien()
{
	
}

/**
 * Returns the language string that names
 * this unit. Each unit type has a unique name.
 * @return Unit name.
 */
std::string RuleAlien::getType() const
{
	return _type;
}

void RuleAlien::setStats(struct UnitStats stats)
{
	_stats = stats;
}


void RuleAlien::setArmor(std::string armor)
{
	_armor = armor;
}

void RuleAlien::setVoxelParameters(int standHeight, int kneelHeight, int loftemps)
{
	_standHeight = standHeight;
	_kneelHeight = kneelHeight;
	_loftemps = loftemps;
}

UnitStats RuleAlien::getStats()
{
	return _stats;
}
/// 

int RuleAlien::getStandHeight()
{
	return _standHeight;
}
/// 
int RuleAlien::getKneelHeight()
{
	return _kneelHeight;
}
/// 
int RuleAlien::gotLoftemps()
{
	return _loftemps;
}

std::string RuleAlien::getArmor() const
{
	return _armor;
}

std::string RuleAlien::getRace() const
{
	return _race;
}

std::string RuleAlien::getRank() const
{
	return _rank;
}

}
