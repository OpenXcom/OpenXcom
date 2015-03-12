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
#include "BaseFacility.h"
#include "../Ruleset/RuleBaseFacility.h"
#include "Base.h"

namespace OpenXcom
{

/**
 * Initializes a base facility of the specified type.
 * @param rules Pointer to ruleset.
 * @param base Pointer to base of origin.
 */
BaseFacility::BaseFacility(RuleBaseFacility *rules, Base *base) : _rules(rules), _base(base), _x(-1), _y(-1), _buildTime(0), _craftForDrawing(0)
{
}

/**
 *
 */
BaseFacility::~BaseFacility()
{
}

/**
 * Loads the base facility from a YAML file.
 * @param node YAML node.
 */
void BaseFacility::load(const YAML::Node &node)
{
	_x = node["x"].as<int>(_x);
	_y = node["y"].as<int>(_y);
	_buildTime = node["buildTime"].as<int>(_buildTime);
}

/**
 * Saves the base facility to a YAML file.
 * @return YAML node.
 */
YAML::Node BaseFacility::save() const
{
	YAML::Node node;
	node["type"] = _rules->getType();
	node["x"] = _x;
	node["y"] = _y;
	if (_buildTime != 0)
		node["buildTime"] = _buildTime;
	return node;
}

/**
 * Returns the ruleset for the base facility's type.
 * @return Pointer to ruleset.
 */
RuleBaseFacility *BaseFacility::getRules() const
{
	return _rules;
}

/**
 * Returns the base facility's X position on the
 * base grid that it's placed on.
 * @return X position in grid squares.
 */
int BaseFacility::getX() const
{
	return _x;
}

/**
 * Changes the base facility's X position on the
 * base grid that it's placed on.
 * @param x X position in grid squares.
 */
void BaseFacility::setX(int x)
{
	_x = x;
}

/**
 * Returns the base facility's Y position on the
 * base grid that it's placed on.
 * @return Y position in grid squares.
 */
int BaseFacility::getY() const
{
	return _y;
}

/**
 * Changes the base facility's Y position on the
 * base grid that it's placed on.
 * @param y Y position in grid squares.
 */
void BaseFacility::setY(int y)
{
	_y = y;
}

/**
 * Returns the base facility's remaining time
 * until it's finished building (0 = complete).
 * @return Time left in days.
 */
int BaseFacility::getBuildTime() const
{
	return _buildTime;
}

/**
 * Changes the base facility's remaining time
 * until it's finished building.
 * @param time Time left in days.
 */
void BaseFacility::setBuildTime(int time)
{
	_buildTime = time;
}

/**
 * Handles the facility building every day.
 */
void BaseFacility::build()
{
	_buildTime--;
}

/**
 * Returns if this facility is currently being
 * used by its base.
 * @return True if it's under use, False otherwise.
 */
bool BaseFacility::inUse() const
{
	if (_buildTime > 0)
	{
		return false;
	}
	return ((_rules->getPersonnel() > 0 && _base->getAvailableQuarters() - _rules->getPersonnel() < _base->getUsedQuarters()) ||
			(_rules->getStorage() > 0 && _base->getAvailableStores() - _rules->getStorage() < _base->getUsedStores()) ||
			(_rules->getLaboratories() > 0 && _base->getAvailableLaboratories() - _rules->getLaboratories() < _base->getUsedLaboratories()) ||
			(_rules->getWorkshops() > 0 && _base->getAvailableWorkshops() - _rules->getWorkshops() < _base->getUsedWorkshops()) ||
			(_rules->getCrafts() > 0 && _base->getAvailableHangars() - _rules->getCrafts() < _base->getUsedHangars()) ||
			(_rules->getPsiLaboratories() > 0 && _base->getAvailablePsiLabs() - _rules->getPsiLaboratories() < _base->getUsedPsiLabs()) ||
			(_rules->getAliens() > 0 && _base->getAvailableContainment() - _rules->getAliens() < _base->getUsedContainment()));
}

/**
 * Gets craft, used for drawing facility.
 * @return craft
 */
Craft *BaseFacility::getCraft() const
{
	return _craftForDrawing;
}

/**
 * Sets craft, used for drawing facility.
 * @param craft for drawing hangar.
 */
void BaseFacility::setCraft(Craft *craft)
{
	_craftForDrawing = craft;
}

}
