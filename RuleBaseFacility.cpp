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
#include "RuleBaseFacility.h"

RuleBaseFacility::RuleBaseFacility(LangString type) : _type(type), _spriteShape(0), _spriteFacility(0), _lift(false), _size(1), _buildCost(0), _buildTime(0), _monthlyCost(0), _storage(0), _personnel(0), _aliens(0), _crafts(0), _labs(0), _workshops(0), _psiLabs(0), _radarRange(0), _radarChance(0), _defence(0)
{
}

RuleBaseFacility::~RuleBaseFacility()
{
}

LangString RuleBaseFacility::getType()
{
	return _type;
}

int RuleBaseFacility::getSpriteShape()
{
	return _spriteShape;
}

void RuleBaseFacility::setSpriteShape(int sprite)
{
	_spriteShape = sprite;
}

int RuleBaseFacility::getSpriteFacility()
{
	return _spriteFacility;
}

void RuleBaseFacility::setSpriteFacility(int sprite)
{
	_spriteFacility = sprite;
}

int RuleBaseFacility::getSize()
{
	return _size;
}

void RuleBaseFacility::setSize(int size)
{
	_size = size;
}

void RuleBaseFacility::setLift(bool lift)
{
	_lift = lift;
}

void RuleBaseFacility::setBuildCost(int cost)
{
	_buildCost = cost;
}

void RuleBaseFacility::setBuildTime(int time)
{
	_buildTime = time;
}

void RuleBaseFacility::setMonthlyCost(int cost)
{
	_monthlyCost = cost;
}

void RuleBaseFacility::setStorage(int storage)
{
	_storage = storage;
}

void RuleBaseFacility::setPersonnel(int personnel)
{
	_personnel = personnel;
}

void RuleBaseFacility::setAliens(int aliens)
{
	_aliens = aliens;
}

void RuleBaseFacility::setCrafts(int crafts)
{
	_crafts = crafts;
}

void RuleBaseFacility::setLaboratories(int labs)
{
	_labs = labs;
}

void RuleBaseFacility::setWorkshops(int workshops)
{
	_workshops = workshops;
}

void RuleBaseFacility::setPsiLaboratories(int psi)
{
	_psiLabs = psi;
}

void RuleBaseFacility::setRadarRange(int range)
{
	_radarRange = range;
}

void RuleBaseFacility::setRadarChance(int chance)
{
	_radarChance = chance;
}

void RuleBaseFacility::setDefenceValue(int defence)
{
	_defence = defence;
}

void RuleBaseFacility::setHitRatio(int ratio)
{
	_hitRatio = ratio;
}