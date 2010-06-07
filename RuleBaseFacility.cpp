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

bool RuleBaseFacility::getLift()
{
	return _lift;
}

void RuleBaseFacility::setLift(bool lift)
{
	_lift = lift;
}

int RuleBaseFacility::getBuildCost()
{
	return _buildCost;
}

void RuleBaseFacility::setBuildCost(int cost)
{
	_buildCost = cost;
}

int RuleBaseFacility::getBuildTime()
{
	return _buildTime;
}

void RuleBaseFacility::setBuildTime(int time)
{
	_buildTime = time;
}

int RuleBaseFacility::getMonthlyCost()
{
	return _monthlyCost;
}

void RuleBaseFacility::setMonthlyCost(int cost)
{
	_monthlyCost = cost;
}

int RuleBaseFacility::getStorage()
{
	return _storage;
}

void RuleBaseFacility::setStorage(int storage)
{
	_storage = storage;
}

int RuleBaseFacility::getPersonnel()
{
	return _personnel;
}

void RuleBaseFacility::setPersonnel(int personnel)
{
	_personnel = personnel;
}

int RuleBaseFacility::getAliens()
{
	return _aliens;
}

void RuleBaseFacility::setAliens(int aliens)
{
	_aliens = aliens;
}

int RuleBaseFacility::getCrafts()
{
	return _crafts;
}

void RuleBaseFacility::setCrafts(int crafts)
{
	_crafts = crafts;
}

int RuleBaseFacility::getLaboratories()
{
	return _labs;
}

void RuleBaseFacility::setLaboratories(int labs)
{
	_labs = labs;
}

int RuleBaseFacility::getWorkshops()
{
	return _workshops;
}

void RuleBaseFacility::setWorkshops(int workshops)
{
	_workshops = workshops;
}

int RuleBaseFacility::getPsiLaboratories()
{
	return _psiLabs;
}

void RuleBaseFacility::setPsiLaboratories(int psi)
{
	_psiLabs = psi;
}

int RuleBaseFacility::getRadarRange()
{
	return _radarRange;
}

void RuleBaseFacility::setRadarRange(int range)
{
	_radarRange = range;
}

int RuleBaseFacility::getRadarChance()
{
	return _radarChance;
}

void RuleBaseFacility::setRadarChance(int chance)
{
	_radarChance = chance;
}

int RuleBaseFacility::getDefenceValue()
{
	return _defence;
}

void RuleBaseFacility::setDefenceValue(int defence)
{
	_defence = defence;
}

int RuleBaseFacility::getHitRatio()
{
	return _hitRatio;
}

void RuleBaseFacility::setHitRatio(int ratio)
{
	_hitRatio = ratio;
}