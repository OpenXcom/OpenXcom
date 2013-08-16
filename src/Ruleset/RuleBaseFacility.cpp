/*
 * Copyright 2010-2013 OpenXcom Developers.
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

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of base facility.
 * @param type String defining the type.
 */
RuleBaseFacility::RuleBaseFacility(const std::string &type) : _type(type), _requires(), _spriteShape(-1), _spriteFacility(-1), _lift(false), _hyper(false), _mind(false), _grav(false), _size(1), _buildCost(0), _buildTime(0), _monthlyCost(0), _storage(0), _personnel(0), _aliens(0), _crafts(0), _labs(0), _workshops(0), _psiLabs(0), _radarRange(0), _radarChance(0), _defense(0), _hitRatio(0), _fireSound(0), _hitSound(0), _listOrder(0)
{
}

/**
 *
 */
RuleBaseFacility::~RuleBaseFacility()
{
}

/**
 * Loads the base facility type from a YAML file.
 * @param node YAML node.
 * @param modIndex offsets the sounds and sprite values to avoid conflicts.
 * @param listOrder the list weight for this facility.
 */
void RuleBaseFacility::load(const YAML::Node &node, int modIndex, int listOrder)
{
	_type = node["type"].as<std::string>(_type);
	_requires = node["requires"].as<std::vector<std::string>>(_requires);
	_spriteShape = node["spriteShape"].as<int>(_spriteShape);
	_spriteFacility = node["spriteFacility"].as<int>(_spriteFacility);
	// BASEBITS.PCK: 34 entries
	if (_spriteFacility > 33)
		_spriteFacility += modIndex;
	_lift = node["lift"].as<bool>(_lift);
	_hyper = node["hyper"].as<bool>(_hyper);
	_mind = node["mind"].as<bool>(_mind);
	_grav = node["grav"].as<bool>(_grav);
	_size = node["size"].as<int>(_size);
	_buildCost = node["buildCost"].as<int>(_buildCost);
	_buildTime = node["buildTime"].as<int>(_buildTime);
	_monthlyCost = node["monthlyCost"].as<int>(_monthlyCost);
	_storage = node["storage"].as<int>(_storage);
	_personnel = node["personnel"].as<int>(_personnel);
	_aliens = node["aliens"].as<int>(_aliens);
	_crafts = node["crafts"].as<int>(_crafts);
	_labs = node["labs"].as<int>(_labs);
	_workshops = node["workshops"].as<int>(_workshops);
	_psiLabs = node["psiLabs"].as<int>(_psiLabs);
	_radarRange = node["radarRange"].as<int>(_radarRange);
	_radarChance = node["radarChance"].as<int>(_radarChance);
	_defense = node["defense"].as<int>(_defense);
	_hitRatio = node["hitRatio"].as<int>(_hitRatio);
	_fireSound = node["fireSound"].as<int>(_fireSound);
	// GEO.CAT: 14 entries
	if (_fireSound > 13)
		_fireSound += modIndex;
	_hitSound = node["hitSound"].as<int>(_hitSound);
	// GEO.CAT: 14 entries
	if (_hitSound > 13)
		_hitSound += modIndex;
	_mapName = node["mapName"].as<std::string>(_mapName);
	_listOrder = node["listOrder"].as<int>(_listOrder);
	if (!_listOrder)
	{
		_listOrder = listOrder;
	}
}

/**
 * Returns the language string that names
 * this base facility. Each base facility type
 * has a unique name.
 * @return Facility name.
 */
std::string RuleBaseFacility::getType() const
{
	return _type;
}

/**
 * Returns the list of research required to
 * build this base facility.
 * @return List of research IDs..
 */
const std::vector<std::string> &RuleBaseFacility::getRequirements() const
{
	return _requires;
}

/**
 * Returns the ID of the sprite used to draw the
 * base structure of the facility that defines its shape.
 * @return Sprite ID.
 */
int RuleBaseFacility::getSpriteShape() const
{
	return _spriteShape;
}

/**
 * Returns the ID of the sprite used to draw the
 * facility's contents inside the base shape.
 * @return Sprite ID.
 */
int RuleBaseFacility::getSpriteFacility() const
{
	return _spriteFacility;
}

/**
 * Returns the size of the facility on the base grid.
 * @return Length in grid squares.
 */
int RuleBaseFacility::getSize() const
{
	return _size;
}

/**
 * Returns whether this facility is the core access lift
 * of a base. Every base has an access lift and all
 * facilities have to be connected to it.
 * @return True if it's a lift, False otherwise.
 */
bool RuleBaseFacility::isLift() const
{
	return _lift;
}

/**
 * Returns whether this facility has hyperwave detection
 * capabilities. This allows it to get extra details about UFOs.
 * @return Hyperwave flag.
 */
bool RuleBaseFacility::isHyperwave() const
{
	return _hyper;
}

/**
 * Returns whether this facility has a mind shield,
 * which covers your base from alien detection.
 * @return Mind Shield flag.
 */
bool RuleBaseFacility::isMindShield() const
{
	return _mind;
}

/**
 * Returns whether this facility has a grav shield,
 * which doubles base defense's fire ratio.
 * @return Grav Shield flag.
 */
bool RuleBaseFacility::isGravShield() const
{
	return _grav;
}

/**
 * Returns the amount of funds that this facility costs
 * to build on a base.
 * @return Building cost.
 */
int RuleBaseFacility::getBuildCost() const
{
	return _buildCost;
}

/**
 * Returns the amount of time that this facility takes
 * to be constructed since placement.
 * @return Time in days.
 */
int RuleBaseFacility::getBuildTime() const
{
	return _buildTime;
}

/**
 * Returns the amount of funds this facility costs monthly
 * to maintain once it's fully built.
 * @return Monthly cost.
 */
int RuleBaseFacility::getMonthlyCost() const
{
	return _monthlyCost;
}

/**
 * Returns the amount of storage space this facility provides
 * for base equipment.
 * @return Storage space.
 */
int RuleBaseFacility::getStorage() const
{
	return _storage;
}

/**
 * Returns the amount of base personnel (soldiers, scientists,
 * engineers) const this facility can contain.
 * @return Amount of personnel.
 */
int RuleBaseFacility::getPersonnel() const
{
	return _personnel;
}

/**
 * Returns the amount of captured live aliens this facility
 * can contain.
 * @return Amount of aliens.
 */
int RuleBaseFacility::getAliens() const
{
	return _aliens;
}

/**
 * Returns the amount of base craft this facility can contain.
 * @return Amount of craft.
 */
int RuleBaseFacility::getCrafts() const
{
	return _crafts;
}

/**
 * Returns the amount of laboratory space this facility provides
 * for research projects.
 * @return Laboratory space.
 */
int RuleBaseFacility::getLaboratories() const
{
	return _labs;
}

/**
 * Returns the amount of workshop space this facility provides
 * for manufacturing projects.
 * @return Workshop space.
 */
int RuleBaseFacility::getWorkshops() const
{
	return _workshops;
}

/**
 * Returns the amount of soldiers this facility can contain
 * for monthly psi-training.
 * @return Amount of soldiers.
 */
int RuleBaseFacility::getPsiLaboratories() const
{
	return _psiLabs;
}

/**
 * Returns the radar range this facility provides for the
 * detection of UFOs.
 * @return Range in nautical miles.
 */
int RuleBaseFacility::getRadarRange() const
{
	return _radarRange;
}

/**
 * Returns the chance of UFOs that come within the facility's
 * radar range to be detected.
 * @return Chance in percentage.
 */
int RuleBaseFacility::getRadarChance() const
{
	return _radarChance;
}

/**
 * Returns the defense value of this facility's weaponry
 * against UFO invasions on the base.
 * @return Defense value.
 */
int RuleBaseFacility::getDefenseValue() const
{
	return _defense;
}

/**
 * Returns the hit ratio of this facility's weaponry
 * against UFO invasions on the base.
 * @return Ratio in percentage.
 */
int RuleBaseFacility::getHitRatio() const
{
	return _hitRatio;
}

/**
 * Returns the battlescape map block name for this facility
 * to construct the base defense mission map.
 * @return Map name.
 */
std::string RuleBaseFacility::getMapName() const
{
	return _mapName;
}

/**
 * Returns the hit sound of this facility's weaponry
 * @return Sound index number.
 */
int RuleBaseFacility::getHitSound() const
{
	return _hitSound;
}

/**
 * Returns the fire sound of this facility's weaponry
 * @return Sound index number.
 */
int RuleBaseFacility::getFireSound() const
{
	return _fireSound;
}

/**
 * @return the list weight for this research item.
 */
int RuleBaseFacility::getListOrder() const
{
	return _listOrder;
}
}
