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
RuleAlien::RuleAlien(const std::string &type, std::string race, std::string rank) : _type(type), _race(race), _rank(rank), _armor(""), _standHeight(0), _kneelHeight(0), _loftemps(0), _value(0)
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


void RuleAlien::setArmor(const std::string &armor)
{
	_armor = armor;
}

void RuleAlien::setVoxelParameters(int standHeight, int kneelHeight, int loftemps)
{
	_standHeight = standHeight;
	_kneelHeight = kneelHeight;
	_loftemps = loftemps;
}

UnitStats RuleAlien::getStats() const
{
	return _stats;
}
/// 

int RuleAlien::getStandHeight() const
{
	return _standHeight;
}
/// 
int RuleAlien::getKneelHeight() const
{
	return _kneelHeight;
}
/// 
int RuleAlien::getLoftemps() const
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

void RuleAlien::setValue(int value)
{
	_value = value;
}

int RuleAlien::getValue() const
{
	return _value;
}


}
